#pragma once

#include "event.h"
#include <sstream>

namespace engine_core {

	class ENGINE_CORE_API MouseMovedEvent : public Event {
	public:
		MouseMovedEvent(float x, float y) : m_mouse_x(x), m_mouse_y(y) {}

		float get_x() const { return m_mouse_x; }
		float get_y() const { return m_mouse_y; }

		std::string to_string() const override {
			std::ostringstream oss;
			oss << "MouseMovedEvent: x=" << m_mouse_x << " y=" << m_mouse_y;
			return oss.str();
		}

		EVENT_CLASS_TYPE(MouseMoved)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

	private:
		float m_mouse_x, m_mouse_y;
	};

	class ENGINE_CORE_API MouseScrolledEvent : public Event {
	public:
		MouseScrolledEvent(float x_offset, float y_offset)
			: m_x_offset(x_offset), m_y_offset(y_offset) {}

		float get_x_offset() const { return m_x_offset; }
		float get_y_offset() const { return m_y_offset; }

		std::string to_string() const override {
			std::ostringstream oss;
			oss << "MouseScrolledEvent: x_offset=" << m_x_offset
				<< " y_offset=" << m_y_offset;
			return oss.str();
		}

		EVENT_CLASS_TYPE(MouseScrolled)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

	private:
		float m_x_offset, m_y_offset;
	};

	class ENGINE_CORE_API MouseButtonEvent : public Event {
	protected:
		MouseButtonEvent(int button) : m_button(button) {}

	public:
		int get_mouse_button() const { return m_button; }

		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

	private:
		int m_button;
	};

	class ENGINE_CORE_API MouseButtonPressedEvent : public MouseButtonEvent {
	public:
		MouseButtonPressedEvent(int button) : MouseButtonEvent(button) {}

		std::string to_string() const override {
			std::ostringstream oss;
			oss << "MouseButtonPressedEvent: button=" << get_mouse_button();
			return oss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonPressed)
	};

	class ENGINE_CORE_API MouseButtonReleasedEvent : public MouseButtonEvent {
	public:
		MouseButtonReleasedEvent(int button) : MouseButtonEvent(button) {}

		std::string to_string() const override {
			std::ostringstream oss;
			oss << "MouseButtonReleasedEvent: button=" << get_mouse_button();
			return oss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonReleased)
	};

} // namespace engine_core