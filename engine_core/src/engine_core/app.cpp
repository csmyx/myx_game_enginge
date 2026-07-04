#include "app.h"
#include "imgui_layer.h"
#include "log.h"

namespace engine_core {

App::~App() = default;

void App::run() {
  ENGINE_CORE_WARN("App running... (close window or press ESC to exit)");
  // 1. Create window — must happen before any layer accesses it
  m_window.reset(Window::create(WindowProps("Myx Engine", 1280, 720)));
  if (!m_window || m_window->should_close()) {
    ENGINE_CORE_ERROR("FATAL: Window creation failed, aborting");
    return;
  }
  m_window->set_event_callback([this](Event &e) { on_event(e); });
  m_last_frame_time = std::chrono::steady_clock::now();

  // 2. ImGui overlay (needs window for OpenGL context)
  m_imgui_layer = new ImGuiLayer(*this);
  push_overlay(m_imgui_layer);

  // 3. Attach all layers — now window & ImGui are ready
  m_layer_stack.attach_all();
  on_init();

  // 4. Main loop
  while (m_running && !m_window->should_close()) {
    auto now = std::chrono::steady_clock::now();
    float dt = std::chrono::duration<float>(now - m_last_frame_time).count();
    m_last_frame_time = now;

    m_window->on_update();       // poll input events
    on_update();                 // app-level per-frame work
    m_layer_stack.on_update(dt); // layer updates (forward order)

    m_window->clear();

    m_imgui_layer->begin();
    m_layer_stack.on_imgui_render(); // layers draw their ImGui widgets
    m_imgui_layer->end();

    m_window->swap_buffers();
  }

  ENGINE_CORE_INFO("App::run() finished");
}

} // namespace engine_core