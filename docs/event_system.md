# Event System

`engine_core` 提供了一套 header-only 的事件系统，用于引擎内部和客户端之间的消息传递。

## 架构概览

```
event.h
  ├── EventType        (事件类型枚举)
  ├── EventCategory    (事件分类位掩码)
  ├── Event            (抽象基类)
  ├── EVENT_CLASS_TYPE (宏：批量生成 get_static_type / get_event_type / get_name)
  └── EVENT_CLASS_CATEGORY (宏：生成 get_category_flags)

key_event.h
  ├── KeyEvent              (键盘事件基类，含 key_code)
  ├── KeyPressedEvent       (按键按下，含 repeat_count)
  ├── KeyReleasedEvent      (按键释放)
  └── KeyTypedEvent         (字符输入)

mouse_event.h
  ├── MouseMovedEvent           (鼠标移动，含 x/y)
  ├── MouseScrolledEvent        (滚轮，含 x/y offset)
  ├── MouseButtonEvent          (鼠标按键基类，含 button)
  ├── MouseButtonPressedEvent   (按键按下)
  └── MouseButtonReleasedEvent  (按键释放)

window_event.h
  ├── WindowResizeEvent    (窗口大小变化，含 width/height)
  └── WindowCloseEvent     (窗口关闭)

event_dispatcher.h
  └── EventDispatcher      (模板分发器，将事件路由到对应回调)
```

## 核心类

### Event（抽象基类）

所有事件的基类，定义在 `event/event.h` 中：

| 方法 | 说明 |
|---|---|
| `get_event_type()` | 返回 `EventType` 枚举值 |
| `get_name()` | 返回事件名称字符串 |
| `get_category_flags()` | 返回分类位掩码（可组合） |
| `to_string()` | 返回可读字符串，默认实现返回 `get_name()` |
| `is_in_category(category)` | 检查事件是否属于某分类 |
| `is_handled()` | 是否已被处理 |
| `set_handled(bool)` | 标记已处理（分发后自动设置） |

### EventType 枚举

```cpp
enum class EventType {
    None = 0,
    WindowResize, WindowClose,
    MouseMoved, MouseScrolled, MouseButtonPressed, MouseButtonReleased,
    KeyPressed, KeyReleased, KeyTyped
};
```

### EventCategory 位掩码

```cpp
enum EventCategory {
    None                  = 0,
    EventCategoryWindow   = 1 << 0,
    EventCategoryMouse    = 1 << 1,
    EventCategoryKeyboard = 1 << 2,
    EventCategoryInput    = 1 << 3
};
```

用法：`event.is_in_category(EventCategoryMouse | EventCategoryKeyboard)` — 匹配鼠标或键盘事件。

## 事件类定义模式

每个具体事件类使用两个宏消除样板代码：

```cpp
class KeyPressedEvent : public KeyEvent {
public:
    KeyPressedEvent(int key_code, int repeat_count) : KeyEvent(key_code), ... {}

    EVENT_CLASS_TYPE(KeyPressed)                          // 生成 get_static_type / get_event_type / get_name
    EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)  // 生成 get_category_flags
};
```

`EVENT_CLASS_TYPE(Name)` 展开为：
```cpp
static EventType get_static_type() { return EventType::Name; }
EventType get_event_type() const override { return get_static_type(); }
const char* get_name() const override { return "Name"; }
```

`EventDispatcher` 依赖 `get_static_type()` 来做编译期类型匹配。

### 各事件类数据成员

| 事件类 | 数据 | getter |
|---|---|---|
| `KeyPressedEvent` | `key_code`, `repeat_count` | `get_key_code()`, `get_repeat_count()` |
| `KeyReleasedEvent` | `key_code` | `get_key_code()` |
| `KeyTypedEvent` | `key_code` | `get_key_code()` |
| `MouseMovedEvent` | `x`, `y` | `get_x()`, `get_y()` |
| `MouseScrolledEvent` | `x_offset`, `y_offset` | `get_x_offset()`, `get_y_offset()` |
| `MouseButton*Event` | `button` | `get_mouse_button()` |
| `WindowResizeEvent` | `width`, `height` | `get_width()`, `get_height()` |
| `WindowCloseEvent` | — | — |

## EventDispatcher

模板分发器，将事件按类型路由到对应的回调函数。定义在 `event/event_dispatcher.h`。

```cpp
EventDispatcher dispatcher(event);

dispatcher.dispatch<KeyPressedEvent>([](KeyPressedEvent& e) {
    // 处理按键按下
    return true;   // true = 标记事件已处理，不再继续分发
});

dispatcher.dispatch<MouseMovedEvent>([](MouseMovedEvent& e) {
    LOG("Mouse at ({}, {})", e.get_x(), e.get_y());
    return false;  // false = 未处理，事件可继续传递
});
```

### 工作原理

```cpp
template<typename T, typename F>
bool dispatch(const F& func) {
    if (m_event.get_event_type() == T::get_static_type()) {
        m_event.m_handled |= func(static_cast<T&>(m_event));
        return true;                             // 类型匹配，已分发
    }
    return false;                                // 类型不匹配
}
```

1. 比较运行时事件类型与目标的 `get_static_type()`
2. 如果匹配，`static_cast` 到具体类型并调用回调
3. 回调返回 `true` 时，自动设置 `m_handled = true`

## 典型用法（App::on_event）

```cpp
void MyApp::on_event(Event& e) {
    EventDispatcher dispatcher(e);

    dispatcher.dispatch<WindowCloseEvent>([this](WindowCloseEvent&) {
        m_running = false;
        return true;
    });

    dispatcher.dispatch<KeyPressedEvent>([](KeyPressedEvent& e) {
        if (e.get_key_code() == VK_ESCAPE) {
            // 退出
            return true;
        }
        return false;
    });

    // 未处理的事件继续传递
    if (!e.is_handled()) {
        // 默认处理逻辑
    }
}
```

## 扩展新事件类型

1. 在 `EventType` 枚举中添加新类型
2. 在 `EventCategory` 中添加新分类位（如需要）
3. 创建事件类，继承 `Event` 或适当的中间基类
4. 使用 `EVENT_CLASS_TYPE` 和 `EVENT_CLASS_CATEGORY` 宏
5. 在 `engine_core.h` 和 `.vcxproj` 中注册头文件

示例：
```cpp
// gamepad_event.h
#pragma once
#include "event.h"

class GamepadButtonPressedEvent : public Event {
public:
    GamepadButtonPressedEvent(int controller_id, int button)
        : m_controller_id(controller_id), m_button(button) {}

    int get_controller_id() const { return m_controller_id; }
    int get_button() const { return m_button; }

    EVENT_CLASS_TYPE(GamepadButtonPressed)
    EVENT_CLASS_CATEGORY(EventCategoryInput)
private:
    int m_controller_id, m_button;
};
```
