#pragma once

#include "event.h"
#include <sstream>

namespace engine_core {

	class ENGINE_CORE_API WindowResizeEvent : public Event {
	public:
		WindowResizeEvent(unsigned int width, unsigned int height)
			: m_width(width), m_height(height) {}

		unsigned int get_width() const { return m_width; }
		unsigned int get_height() const { return m_height; }

		std::string to_string() const override {
			std::ostringstream oss;
			oss << "WindowResizeEvent: " << m_width << "x" << m_height;
			return oss.str();
		}

		EVENT_CLASS_TYPE(WindowResize)
		EVENT_CLASS_CATEGORY(EventCategoryWindow)

	private:
		unsigned int m_width, m_height;
	};

	class ENGINE_CORE_API WindowCloseEvent : public Event {
	public:
		WindowCloseEvent() = default;

		std::string to_string() const override {
			return "WindowCloseEvent";
		}

		EVENT_CLASS_TYPE(WindowClose)
		EVENT_CLASS_CATEGORY(EventCategoryWindow)
	};

} // namespace engine_core
