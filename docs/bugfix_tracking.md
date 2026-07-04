# Bugfix 跟踪记录

按时间倒序，记录关键 bug 的根因、修复和 commit。

---

## 12. ImGui 运行时崩溃 (0x00000001)

**Commit**: `5898846` — fix: switch imgui to fully self-compiled source, fix DLL export

**现象**: `ImGui::Begin()` 内崩溃，地址 `0x00000001`

**根因**: imgui 是静态库链接进 DLL，`IMGUI_API` 默认为空。sandbox 调用 `ImGui::Begin()` 时访问的是自己那份 null ImGuiContext。

**修复**: 
- 完全自编译 imgui 源码（head+src），移除 vcpkg imgui 依赖
- `IMGUI_API` 设为 `__declspec(dllexport)`（编译 DLL 时）/ `__declspec(dllimport)`（消费时）
- CMakeLists.txt + vcxproj 全局注入 `IMGUI_API=__declspec(dllexport)` 编译宏

---

## 11. `get_window()` 在 `on_attach()` 中解引用空指针

**Commit**: `9788d14` — fix: defer on_attach() until window is created

**现象**: `SandboxLayer::on_attach()` 调用 `get_window().set_clear_color()` 崩溃

**根因**: `push_layer()` 立即触发 `on_attach()`，但窗口要等到 `App::run()` 里才创建。

**修复**: 
- `push_layer`/`push_overlay` 只存指针，不调 `on_attach()`
- 新增 `LayerStack::attach_all()`，在 `run()` 中窗口创建后统一调用

---

## 10. Layer / get_window 空引用风险

**Commit**: `894703c` — refactor: Layer constructor requires App&

**现象**: Layer 的 `m_app` 可为 null，`get_app()`/`get_window()` 无保护

**根因**: `set_app()` 是公开方法，可能在 Layer 创建后忘记调用

**修复**: 
- 构造函数改为 `Layer(App& app)`，强制注入
- 移除 `friend class LayerStack` 和 `set_app()`

---

## 9. `#include <glad/glad.h>` 的两种冲突

**Commit**: `399b39a` — fix: remove glad from public header to fix null-pointer crash

**两个子问题**:

### 9a. `glClearColor` 崩溃 (0x00000000)

**现象**: `glClearColor(0.2f, 0.3f, 0.4f, 1.0f)` 跳转 `0x00000000`

**根因**: glad 是静态库链接进 DLL，函数指针内部有效。但 `<glad/glad.h>` 暴露给 sandbox，sandbox 编译时产生自己那份 glad 函数指针——全是 null。

**修复**: glad 完全藏在 DLL 内，只通过 `Window` 接口暴露 `clear()` / `set_clear_color()` / `swap_buffers()`

### 9b. `glad.h(27): error C1189: OpenGL header already included`

**现象**: 编译错误 "OpenGL header already included, remove this include, glad already provides it"

**根因**: `GLFW/glfw3.h` 默认 include `<GL/gl.h>`，glad 也提供 OpenGL 声明，冲突。

**修复**: 所有 include GLFW 之前定义 `GLFW_INCLUDE_NONE`。include 顺序： `glad/glad.h` → `GLFW_INCLUDE_NONE` → `GLFW/glfw3.h`

---

## 8. `entry_point.h` 链接错误

**Commit**: 早期的 vcxproj 修复

**现象**: `unresolved external symbol create_app` — engine_core.dll 链接失败

**根因**: `entry_point.h` 在 vcxproj 中列为 `<ClCompile>`。因最初是 `entry_point.cpp`，改名后 vcxproj 未更新。

**修复**: 把 `entry_point.h` 从 `<ClCompile>` 移到 `<ClInclude>`（在 IDE 中右键改名才会自动更新）

---

## 7. Ninja 链接器错误 (`ld` 替代 `link.exe`)

**Commit**: `ad3a2cf` — fix: resolve Ninja linker issue (ld vs link.exe) and add CMake presets

**现象**: `ld: error: undefined symbol: __declspec(dllimport) spdlog::details::...`

**根因**: PATH 里有 MSYS2 的 `ld.exe`，没有 MSVC 的 `link.exe`。CMake+Ninja 找不到 MSVC 链接器就回退到了 GNU `ld`。

**修复**:
- 创建 `CMakePresets.json`，Ninja preset 显式指定 `CMAKE_MAKE_PROGRAM`
- 文档说明：必须从 VS Developer Command Prompt 构建 Ninja
- 创建 `scripts/setup-vs-env.nu` 给 Nushell 用户

---

## 6. Release 构建失败 — `std::println`

**Commit**: 早于窗口系统实现的提交

**现象**: Release 构建报错 `"println": 不是 "std" 的成员`

**根因**: `test.cpp` 使用 `std::println`（C++23 特性），但 Release 配置是 C++20。

**修复**: `std::println` → `std::cout`

---

## 5. `layer.cpp` 未编译进 vcxproj

**Commit**: `399b39a` 附近

**现象**: MSBuild 链接错误 `unresolved external symbol Layer::~Layer`

**根因**: CMake 用 GLOB_RECURSE 自动拾取，vcxproj 需手动添加

**修复**: 在 vcxproj 中添加 `<ClCompile Include="src\engine_core\layer.cpp" />` 和对应的 `.h`

---

## 4. `MyLayer2` 调用不存在的 `close()`

**Commit**: `6954466` — refactor: clarify App/Layer/Event architecture

**现象**: 编译错误 `'close': identifier not found`

**根因**: `MyLayer2` 继承 `Layer` 却调用了 `App::close()`

**修复**: 重写 sandbox：`MyApp2 : public App` 用 App 的 `on_event()`，事件处理逻辑放在 App 而非 Layer

---

## 3. `app.h` 缺少 `extern create_app()`

**Commit**: `6954466` 附近

**现象**: 编译错误 `unresolved external symbol create_app`

**根因**: 重构 `app.h` 时误删了 `extern engine_core::App* create_app()` 声明

**修复**: 该声明在 `entry_point.h` 中已存在（`app.h` 里的是重复）。确认 entry_point.h 的声明有效

---

## 2. `layer.h` circular include

**Commit**: `37ca49f` 之前

**现象**: 编译错误 `use of undefined type 'engine_core::App'`

**根因**: `layer.h` 中 `get_window()` 内联调用 `m_app->get_window()`，需要 App 完整定义。但 `layer.h` 不能 include `app.h`（会循环依赖）。

**修复**: `get_window()` 声明在 `layer.h`，实现在 `layer.cpp`（可 include `app.h`）

---

## 1. CMake configure 失败 — `ENGINE_CORE_API` 未定义

**Commit**: `07fb3e9` 提交后

**现象**: `syntax error: missing ';' before identifier 'ImGuiContext'`

**根因**: `engine_core.h` 中 `#define IMGUI_API ENGINE_CORE_API` 在 `util.h` include 之前，`ENGINE_CORE_API` 尚未定义

**修复**: 改用原始 `__declspec(dllimport/dllexport)` 配合 `ENGINE_CORE_EXPORTS` 宏

---

## 模式总结

**最常见根因**（出现 4 次）: **静态库链接进 DLL 后符号不导出**

| 库 | 问题 | 修复模式 |
|---|---|---|
| glad | 函数指针双重副本 | 藏在 DLL 内，通过 Window 接口暴露 |
| spdlog | 静态链接，无导出问题 | 不变（我们没有调用 spdlog 符号，只是内部用） |
| ImGui | DLL context vs EXE copy | `IMGUI_API = __declspec(dllimport/export)` |

**经验法则**: 如果一个库的**全局状态**（context、函数指针表）被 DLL 和 EXE 共享，它的符号必须走 DLL import/export。
