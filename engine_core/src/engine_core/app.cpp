#include "app.h"
#include "log.h"
#include "event/event_dispatcher.h"

namespace engine_core {

	App::~App() = default;

	void App::run() {
		ENGINE_CORE_INFO("App::run() started");

		m_window.reset(Window::create(WindowProps("Myx Engine", 1280, 720)));
		ENGINE_CORE_ASSERT(m_window, "Window::create() returned nullptr");
		m_window->set_event_callback([this](Event& e) {
			on_event(e);
		});

		while (m_running && !m_window->should_close()) {
			m_window->on_update();
			on_update();
		}

		ENGINE_CORE_INFO("App::run() finished");
	}

} // namespace engine_core