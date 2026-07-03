#pragma once

#include "util.h"
#include "window.h"
#include "event/event.h"
#include <memory>

namespace engine_core {

#pragma warning(push)
#pragma warning(disable: 4251)

	class ENGINE_CORE_API App {
	protected:
		App() {}
	public:
		static App* create_app();
	public:
		virtual ~App();

		void run();

		virtual void on_event(Event& e) {}
		virtual void on_update() {}

		Window& get_window() { return *m_window; }

	private:
		bool m_running = true;
		std::unique_ptr<Window> m_window;
	};

#pragma warning(pop)

	extern engine_core::App* create_app();

} // namespace engine_core
