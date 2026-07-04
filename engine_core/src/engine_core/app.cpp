#include "app.h"
#include "event/event_dispatcher.h"
#include "log.h"

namespace engine_core {

App::~App() = default;

void App::run() {
  ENGINE_CORE_INFO("App::run() started");

  m_window.reset(Window::create(WindowProps("Myx Engine", 1280, 720)));
  if (!m_window || m_window->should_close()) {
    ENGINE_CORE_ERROR("FATAL: Window creation failed, aborting");
    return;
  }
  m_window->set_event_callback([this](Event &e) { on_event(e); });
  m_window->set_clear_color(0.1f, 0.1f, 0.1f, 1.0f); // default dark grey

  while (m_running && !m_window->should_close()) {
    m_window->on_update();
    on_update();

    for (Layer *layer : m_layer_stack) {
      layer->on_update(0.0f);
    }

    m_window->clear();
    m_window->swap_buffers();
  }

  ENGINE_CORE_INFO("App::run() finished");
}

} // namespace engine_core