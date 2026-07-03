# Window System

`engine_core` 提供了一套跨平台的窗口抽象，通过 `platform/` 子目录隔离不同平台的实现。当前仅支持 Windows（基于 GLFW）。

## 架构

```
window.h                    ← 抽象 Window 接口 + WindowProps
platform/
  windows/
    windows_window.h        ← WindowsWindow (GLFW 实现)
    windows_window.cpp      ← 实现: GLFW 初始化、回调翻译、事件分发
  linux/                    ← (未来) X11/Wayland 实现
  mac/                      ← (未来) Cocoa 实现
```

跨平台新增只需：在新 `platform/<os>/` 下实现 `Window` 子类，并在 `Window::create()` 工厂中通过 `#ifdef` 选择。

## 核心类

### Window（抽象基类）

定义在 `window.h`：

| 方法 | 说明 |
|---|---|
| `on_update()` | 每帧调用，轮询窗口事件 |
| `get_width()` / `get_height()` | 当前窗口尺寸 |
| `should_close()` | 窗口是否收到关闭请求 |
| `set_event_callback(fn)` | 设置事件回调，GLFW 事件通过此回调分发 |
| `set_vsync(enabled)` / `is_vsync()` | 垂直同步开关 |
| `get_native_window()` | 返回原生窗口句柄 (`GLFWwindow*`) |
| `static create(props)` | 工厂方法，创建平台相关窗口实例 |

### WindowProps

```cpp
struct WindowProps {
    std::string title   = "Myx Engine";
    unsigned int width  = 1280;
    unsigned int height = 720;
};
```

## GLFW 事件翻译

`WindowsWindow` 将 GLFW 回调翻译为引擎 `Event` 对象：

| GLFW 回调 | 引擎 Event |
|---|---|
| `glfwSetKeyCallback` | `KeyPressedEvent` / `KeyReleasedEvent` |
| `glfwSetMouseButtonCallback` | `MouseButtonPressedEvent` / `MouseButtonReleasedEvent` |
| `glfwSetCursorPosCallback` | `MouseMovedEvent` |
| `glfwSetScrollCallback` | `MouseScrolledEvent` |
| `glfwSetWindowSizeCallback` | `WindowResizeEvent` |
| `glfwSetWindowCloseCallback` | `WindowCloseEvent` |

所有回调通过 `glfwSetWindowUserPointer` 访问 `WindowsWindow` 实例，再调用 `m_event_callback` 将事件传递给 `App::on_event()`。

## 事件流

```
GLFW 原生事件
  → WindowsWindow::<static_callback>()
    → Event 对象构造
      → m_event_callback(event)
        → App::on_event(Event&)
          → EventDispatcher 分发
```

## 主循环

`App::run()` 中的典型流程：

```cpp
void App::run() {
    m_window.reset(Window::create(WindowProps("My Game", 1920, 1080)));
    m_window->set_event_callback([this](Event& e) { on_event(e); });

    while (m_running && !m_window->should_close()) {
        m_window->on_update();   // glfwPollEvents() + 交换缓冲
        on_update();             // 逻辑更新
    }
}
```

## 使用示例

```cpp
class MyApp : public engine_core::App {
    bool m_wireframe = false;

    void on_event(Event& e) override {
        EventDispatcher d(e);
        d.dispatch<WindowCloseEvent>([this](auto&) {
            // 窗口关闭 → 退出主循环
            m_running = false;  // App 基类成员
            return true;
        });
        d.dispatch<WindowResizeEvent>([](auto& e) {
            ENGINE_CORE_INFO("Window resized to {}x{}", e.get_width(), e.get_height());
            return false;
        });
        d.dispatch<KeyPressedEvent>([this](auto& e) {
            if (e.get_key_code() == GLFW_KEY_ESCAPE) {
                m_running = false;
                return true;
            }
            if (e.get_key_code() == GLFW_KEY_F1) {
                m_wireframe = !m_wireframe;
                return true;
            }
            return false;
        });
    }

    void on_update() override {
        // 每帧逻辑
    }
};
```

## GLFW 生命周期

- `glfwInit()` 在首次创建 `WindowsWindow` 时调用（静态 `s_glfw_initialized` 标志保护）
- `glfwDestroyWindow()` 在 `WindowsWindow::~WindowsWindow()` 中调用
- `glfwTerminate()` 留待引擎整体退出时调用（当前未实现，可扩展）

## 扩展到新平台

1. 在 `platform/<os>/` 下创建 `xxx_window.h` / `xxx_window.cpp`
2. 实现 `Window` 所有纯虚方法
3. 在 `Window::create()` 工厂中添加对应 `#ifdef` 分支：

```cpp
Window* Window::create(const WindowProps& props) {
#ifdef _WIN32
    return new WindowsWindow(props);
#elif __linux__
    return new LinuxWindow(props);
#elif __APPLE__
    return new MacWindow(props);
#else
    #error "Unsupported platform"
#endif
}
```
