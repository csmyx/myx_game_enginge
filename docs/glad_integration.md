# Glad 集成说明

## 崩溃根因

```
Exception 0xc0000005 at address 0x00000000
→ glClearColor 函数指针为 null
```

**本质：glad 静态库链接到 DLL 内部，但公开头文件暴露了 glad 声明给外部消费者（sandbox）。导致两边各自有一份 glad 符号副本，sandbox 那份是 null。**

### 内存布局

```
engine_core.dll (DLL)               sandbox.exe (EXE)
┌──────────────────────────┐        ┌──────────────────────────┐
│ glad 静态库 (.lib)       │        │ (没有 glad.lib)          │
│  ├─ glad_glClear = 地址A  │        │                          │
│  ├─ glad_glClearColor=地址B│       │ glClearColor → ???      │
│  └─ ...                   │        │                          │
│                           │        │ 当 engine_core.h         │
│ gladLoadGLLoader() 执行   │        │ include <glad/glad.h>    │
│ → 填充所有指针为真实地址   │        │ 时，编译器看到           │
│                           │        │ extern 声明，           │
│                           │        │ 链接器找不到定义         │
│                           │        │ → 函数指针 = 0x00000000  │
└──────────────────────────┘        └──────────────────────────┘
```

### 为什么 assert 没拦住？

```cpp
if (!gladLoadGLLoader(...))  // 返回 1（成功）
    return;                   // ← 没走到这里

// gladLoadGLLoader 在 DLL 内部确实成功了，
// 填充了 DLL 内部的指针。但 sandbox.exe 里的
// 指针是另一份，永远是 null。
```

glad 是在 `windows_window.cpp`（属于 DLL）里调用的，它填充的是 DLL 数据段的指针。sandbox 编译时看到了 `glad/glad.h` 的声明，链接时企图从 DLL 的导出符号找定义，但 glad 静态库的符号**默认不导出**，所以 sandbox 里每个 GL 函数指针都是 null。

## 修复方案

**glad 是 engine_core 的私有实现细节，不能出现在公开头文件中。**

### 修改前

```
engine_core.h
  ├── #include <glad/glad.h>       ← ❌ 暴露给 sandbox
  ├── #include <GLFW/glfw3.h>
  └── ...

app.cpp / my_app.cpp
  ├── glClearColor(...)             ← ❌ sandbox 直接调 OpenGL
  └── glClear(...)
```

### 修改后

```
engine_core.h
  ├── #include <GLFW/glfw3.h>       ← 只暴露按键码等符号
  └── ...

window.h (Window 接口)
  ├── set_clear_color(r,g,b,a)      ← OpenGL 操作封装为方法
  ├── clear()
  └── swap_buffers()

windows_window.cpp
  ├── #include <glad/glad.h>        ← 唯一引入 glad 的地方
  ├── init() → gladLoadGLLoader()   ← 只在 DLL 内部初始化
  ├── set_clear_color() → glClearColor()
  ├── clear() → glClear()
  └── swap_buffers() → glfwSwapBuffers()
```

### 调用链

```
App::run()
  → on_update()                    // sandbox 业务逻辑
  → m_window->clear()              // WindowsWindow::clear()
      → glClear(GL_COLOR_BUFFER_BIT)  // ← 只有 DLL 内部调 OpenGL
  → m_window->swap_buffers()       // WindowsWindow::swap_buffers()
      → glfwSwapBuffers()
```

所有 OpenGL 调用都在 DLL 内部完成，sandbox 通过 Window 抽象接口使用，永远不碰 glad 符号。

## 经验教训

1. **静态库不应该通过 DLL 公开头文件暴露** — 静态库的符号是 DLL 私有的，消费者看不到
2. **封装平台细节** — OpenGL/glad 是实现细节，应藏在 Window 接口后面
3. **尽量不直接从公开头文件暴露第三方库** — 除非是设计意图（如 GLFW 的按键码常量）
