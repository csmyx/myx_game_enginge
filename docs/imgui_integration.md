# ImGui 集成

## 集成方式

当前采用 **vcpkg + vendored backend** 混合方案：

```
vcpkg.json → imgui (core)
    ↓ 提供
    imgui.h, imgui_internal.h, imgui_stdlib.h  (头文件)
    imgui.lib                                     (静态库)

engine_core/src/engine_core/imgui/
    imgui_impl_glfw.cpp / .h           ← vendor (自管)
    imgui_impl_opengl3.cpp / .h        ← vendor (自管)
    imgui_impl_opengl3_loader.h        ← vendor (自管)
```

### 为什么 backend 必须自己编译？

vcpkg 提供的 imgui backend 特性（`glfw-binding`、`opengl3-binding`）使用系统 OpenGL 头文件。我们的引擎用 **glad** 加载 OpenGL，与系统头文件冲突。

Backend 源码编译时需要定义 `IMGUI_IMPL_OPENGL_LOADER_GLAD`，让 `imgui_impl_opengl3.h` 使用 glad 而非系统 GL：

```cpp
// imgui_impl_opengl3.h (vendored, compiled by us)
#ifdef IMGUI_IMPL_OPENGL_LOADER_GLAD
#include <glad/glad.h>   // ← 我们的路径
#else
#include <GL/gl.h>       // ← 系统路径（冲突）
#endif
```

这个宏通过 `CMakeLists.txt` 和 `vcxproj` 注入：

```cmake
# CMakeLists.txt
target_compile_definitions(engine_core PRIVATE IMGUI_IMPL_OPENGL_LOADER_GLAD)
```

```xml
<!-- vcxproj -->
<ClCompile Include="imgui_impl_opengl3.cpp">
  <PreprocessorDefinitions>IMGUI_IMPL_OPENGL_LOADER_GLAD;%(PreprocessorDefinitions)</PreprocessorDefinitions>
</ClCompile>
```

### 如果切到纯源码方式

只需 5 步：

1. 从 [imgui release](https://github.com/ocornut/imgui/releases) 下载源码
2. 将 `imgui.cpp`、`imgui.h`、`imgui_internal.h` 等复制到 `engine_core/src/engine_core/imgui/`
3. 从 `vcpkg.json` 移除 imgui 条目
4. 从 `CMakeLists.txt` 移除 `find_package(imgui)` 和 `imgui::imgui`
5. GLOB_RECURSE 自动捡到新的 .cpp 文件

不需要改其他任何代码。

## 运行时流程

```
App::run() 启动
  → new ImGuiLayer(*this)
  → push_overlay(imgui_layer)     // ImGui 总在最上层

每帧:
  ImGuiLayer::begin()
    → ImGui_ImplOpenGL3_NewFrame()
    → ImGui_ImplGlfw_NewFrame()
    → ImGui::NewFrame()

  layers.on_imgui_render()        // 各 Layer 画自己的 UI
    → SandboxLayer: ImGui::Begin("Debug"), ImGui::Text(...), ImGui::End()

  ImGuiLayer::end()
    → ImGui::Render()
    → ImGui_ImplOpenGL3_RenderDrawData()
```

## ImGui 事件处理

ImGuiLayer 自动拦截鼠标/键盘事件。当 ImGui 窗口获得焦点时，底层 Layer 不会收到事件：

```cpp
void ImGuiLayer::on_event(Event& e) {
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse || io.WantCaptureKeyboard) {
        e.set_handled(true);  // 阻断事件继续向下传播
    }
}
```

因为 ImGuiLayer 是 overlay（在 LayerStack 最顶层），`on_event` 倒序遍历时会首先经过 ImGuiLayer。如果 ImGui 想要捕获输入，事件就被消费了，游戏 Layer 不会收到。

## 使用示例

任何 Layer 覆写 `on_imgui_render()` 即可：

```cpp
class SandboxLayer : public Layer {
  void on_imgui_render() override {
    ImGui::Begin("Debug");
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    ImGui::Text("Frame time: %.3f ms", 1000.0f / ImGui::GetIO().Framerate);
    ImGui::End();
  }
};

class EditorLayer : public Layer {
  void on_imgui_render() override {
    ImGui::Begin("Scene Hierarchy");
    // ... 场景树 UI
    ImGui::End();

    ImGui::Begin("Properties");
    // ... 属性面板
    ImGui::End();
  }
};
```

每个 Layer 可以创建多个 ImGui 窗口，它们会自动排列。不需要管理窗口布局——ImGui 的 docking 功能（需切到 docking 分支）可以自由拖拽组合。

## 调试建议

- 在 `on_imgui_render()` 开头加 `ImGui::ShowDemoWindow()` 可以看到 ImGui 所有控件的示例
- 使用 `ImGui::GetIO().Framerate` 获取精确帧率，比手动计时更准确
- 重度 UI 场景考虑用 `ImGui::BeginChild` 做子区域分割
