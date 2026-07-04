#pragma once

#include "event.h"

namespace engine_core {

class ENGINE_CORE_API KeyEvent : public Event {
protected:
    KeyEvent(int key_code)
        : m_key_code(key_code)
    {
    }

public:
    int get_key_code() const { return m_key_code; }

    EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)

private:
    int m_key_code;
};

class ENGINE_CORE_API KeyPressedEvent : public KeyEvent {
public:
    KeyPressedEvent(int key_code, int repeat_count)
        : KeyEvent(key_code)
        , m_repeat_count(repeat_count)
    {
    }

    int get_repeat_count() const { return m_repeat_count; }

    std::string to_string() const override
    {
        std::ostringstream oss;
        oss << "KeyPressedEvent: key=" << get_key_code()
            << " repeat=" << m_repeat_count;
        return oss.str();
    }

    EVENT_CLASS_TYPE(KeyPressed)

private:
    int m_repeat_count;
};

class ENGINE_CORE_API KeyReleasedEvent : public KeyEvent {
public:
    KeyReleasedEvent(int key_code)
        : KeyEvent(key_code)
    {
    }

    std::string to_string() const override
    {
        std::ostringstream oss;
        oss << "KeyReleasedEvent: key=" << get_key_code();
        return oss.str();
    }

    EVENT_CLASS_TYPE(KeyReleased)
};

class ENGINE_CORE_API KeyTypedEvent : public KeyEvent {
public:
    KeyTypedEvent(int key_code)
        : KeyEvent(key_code)
    {
    }

    std::string to_string() const override
    {
        std::ostringstream oss;
        oss << "KeyTypedEvent: key=" << get_key_code();
        return oss.str();
    }

    EVENT_CLASS_TYPE(KeyTyped)
};

} // namespace engine_core