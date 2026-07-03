#pragma once

#include "event.h"

namespace engine_core {

	class EventDispatcher {
	public:
		explicit EventDispatcher(Event& event) : m_event(event) {}

		template<typename T, typename F>
		bool dispatch(const F& func) {
			if (m_event.get_event_type() == T::get_static_type()) {
				m_event.m_handled |= func(static_cast<T&>(m_event));
				return true;
			}
			return false;
		}

	private:
		Event& m_event;
	};

} // namespace engine_core
