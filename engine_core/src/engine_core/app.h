#pragma once

#include "event/event.h"
#include "event/event_dispatcher.h"
#include "event/window_event.h"
#include "layer_stack.h"
#include "util.h"
#include "window.h"
#include <memory>
#include <ranges>

namespace engine_core {

#pragma warning(push)
#pragma warning(disable : 4251)

class ENGINE_CORE_API App {
protected:
  App() {}

public:
  static App *create_app();

public:
  virtual ~App();

  void run();
  void close() { m_running = false; }

  virtual void on_event(Event &e) {
    engine_core::EventDispatcher d(e);
    d.dispatch<engine_core::WindowCloseEvent>([this](auto &) {
      close();
      return true;
    });
    for (auto &layer : std::ranges::reverse_view(m_layer_stack)) {
      layer->on_event(e);
      if (e.is_handled())
        break;
    }
  }
  virtual void on_update() {}

  void push_layer(Layer *layer) { m_layer_stack.push_layer(layer); }
  void push_overlay(Layer *overlay) { m_layer_stack.push_overlay(overlay); }

  Window &get_window() { return *m_window; }

private:
  bool m_running = true;
  std::unique_ptr<Window> m_window;
  LayerStack m_layer_stack;
};

#pragma warning(pop)

extern engine_core::App *create_app();

} // namespace engine_core
