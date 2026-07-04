#pragma once

#include "../util.h"
#include <sstream>
#include <string>

namespace engine_core {

enum class EventType {
  None = 0,
  WindowResize,
  WindowClose,
  MouseMoved,
  MouseScrolled,
  MouseButtonPressed,
  MouseButtonReleased,
  KeyPressed,
  KeyReleased,
  KeyTyped
};

enum EventCategory {
  None = 0,
  EventCategoryWindow = 1 << 0,
  EventCategoryMouse = 1 << 1,
  EventCategoryKeyboard = 1 << 2,
  EventCategoryInput = 1 << 3
};

#define EVENT_CLASS_TYPE(type)                                                 \
  static EventType get_static_type() { return EventType::type; }               \
  EventType get_event_type() const override { return get_static_type(); }      \
  const char *get_name() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category)                                         \
  int get_category_flags() const override { return category; }

class ENGINE_CORE_API Event {
public:
  Event() = default;
  virtual ~Event() = default;

  virtual EventType get_event_type() const = 0;
  virtual const char *get_name() const = 0;
  virtual int get_category_flags() const = 0;
  virtual std::string to_string() const { return get_name(); }

  bool is_in_category(int category) const {
    return get_category_flags() & category;
  }

  bool is_handled() const { return m_handled; }
  void set_handled(bool handled) { m_handled = handled; }

private:
  bool m_handled = false;

  friend class EventDispatcher;
};

inline std::ostream &operator<<(std::ostream &os, const Event &e) {
  return os << e.to_string();
}

} // namespace engine_core