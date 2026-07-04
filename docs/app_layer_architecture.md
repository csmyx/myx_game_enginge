# App / Layer 架构

## 层次关系

```
main()
 └─ App (引擎宿主)
     ├── Window (GLFW)           — 私有，App 独占
     ├── LayerStack              — 管理 Layer 生命周期和事件/更新分发
     └── Layers[]                — 用户实现的游戏逻辑
          ├── SandboxLayer
          └── UILayer ...
```

## 职责划分

| 类 | 职责 | 用户是否重写 |
|---|---|---|
| **App** | 主循环、窗口创建销毁、事件转发给 Layer、dt 计算 | 很少重写 |
| **Layer** | 游戏逻辑、UI、渲染 | **主要重写** |
| **LayerStack** | Layer 增删、正向更新、反向事件分发 | 不重写 |
| **Window** | OpenGL 上下文、输入轮询、清屏、交换缓冲 | 不重写 |

## 数据流

### 事件流（倒序，顶层 Overlay 优先）

```
GLFW 回调
 → WindowsWindow 静态回调 → Event 对象
   → App::on_event(Event&)
     ├── WindowCloseEvent → close()      (App 默认处理)
     └── LayerStack::on_event (倒序)
          ├── Overlay N  → on_event(e)   (顶层 UI 优先)
          ├── ...
          └── Layer 0    → on_event(e)   (底层最后)
```

### 更新流（正序，底层先更新）

```
App::run() 每帧:
  ├── m_window->on_update()              // 轮询输入 → 产生 Event
  ├── on_update()                        // App 级 per-frame 工作
  ├── LayerStack::on_update(dt)          // 正序: Layer 0 → Overlay N
  │     ├── Layer 0 → on_update(dt)
  │     └── ...
  ├── m_window->clear()                  // glClear
  ├── LayerStack::on_imgui_render()      // Layer 渲染
  └── m_window->swap_buffers()           // 交换缓冲
```

## Layer 如何访问 Window

Layer 通过 `get_app()` 间接访问 Window，不直接依赖 Window 类：

```cpp
class SandboxLayer : public engine_core::Layer {
  void on_attach() override {
    get_window().set_clear_color(0.2f, 0.3f, 0.4f, 1.0f);
  }
};
```

**原理**：`LayerStack` 在 `push_layer()` 时通过 `set_app()` 将 `App*` 注入 Layer。`get_window()` 实际调用链：

```
Layer::get_window()
 → Layer::get_app()        // 返回 m_app (LayerStack 注入的)
   → App::get_window()     // 返回 *m_window (App 的私有成员)
```

## 哪些类需要 `ENGINE_CORE_API`

| 类 | 原因 |
|---|---|
| `App` | sandbox 的子类需要看到 App 的完整定义 |
| `Layer` | sandbox 的子类需要看到 Layer 的 vtable |
| `Window` | sandbox 通过 `get_window()` 获取引用 |
| `Event` | 事件分发依赖多态 |
| `Log` | 静态方法被 sandbox 调用 |

## 不重写原则

### App 的三个虚函数

| 方法 | 默认行为 | 何时重写 |
|---|---|---|
| `on_init()` | 空 | App 级一次性初始化 |
| `on_update()` | 空 | 替代方案：用 Layer 代替 |
| `on_event()` | 处理 WindowClose + 转发给 Layer | **基本不重写**。如必须，末尾调用 `App::on_event(e)` |

### 为什么尽量不重写 App？

App 的默认 `on_event()` 已经做了最关键的事：**WindowClose → close()**。如果你 override 了 `on_event()` 而不调用基类的，关闭按钮就失效了。

如果你需要处理事件，**add a Layer**，在 Layer 的 `on_event()` 里处理。Layer 之间的事件流用 `e.is_handled()` 控制是否继续向下传递。

## sandbox 完整示例

```cpp
#include <engine_core.h>

class SandboxLayer : public engine_core::Layer {
public:
  SandboxLayer() : Layer("SandboxLayer") {}

  void on_attach() override {
    ENGINE_CLIENT_INFO("SandboxLayer attached");
    get_window().set_clear_color(0.2f, 0.3f, 0.4f, 1.0f);
  }

  void on_event(engine_core::Event& e) override {
    engine_core::EventDispatcher d(e);
    d.dispatch<engine_core::KeyPressedEvent>([](auto& e) {
      if (e.get_key_code() == GLFW_KEY_ESCAPE) {
        ENGINE_CLIENT_INFO("ESC pressed");
      }
      return false;
    });
    d.dispatch<engine_core::MouseButtonReleasedEvent>([this](auto&) {
      // 随机换背景色
      float r = (float)rand() / RAND_MAX;
      float g = (float)rand() / RAND_MAX;
      float b = (float)rand() / RAND_MAX;
      get_window().set_clear_color(r, g, b, 1.0f);
      return true;
    });
  }

  void on_update(float dt) override {
    // 每帧逻辑
  }
};

class SandboxApp : public engine_core::App {
  void on_init() override {
    ENGINE_CLIENT_INFO("SandboxApp initializing...");
  }
};

engine_core::App* create_app() {
  auto* app = new SandboxApp();
  app->push_layer(new SandboxLayer());
  return app;
}
```

## 常见问题

### Q: Layer 需要访问 Window 做渲染怎么办？

`get_window().set_clear_color()` 等 Window 接口方法。

### Q: App::on_update() vs Layer::on_update() 用哪个？

绝大多数情况用 Layer。App::on_update() 只用于 App 级别的全局工作（极少需要）。

### Q: 事件在多个 Layer 间怎么传递？

App::on_event() 按**倒序**遍历 Layer（Overlay 优先）。任一 Layer 调用 `e.set_handled(true)` 后停止传递。

### Q: push_layer vs push_overlay 区别？

- `push_layer` — 普通渲染层，插入到 overlay 之前
- `push_overlay` — UI 覆盖层，总是在最上层渲染，事件优先级最高
