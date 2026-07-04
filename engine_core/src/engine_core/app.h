#pragma once

#include "event/event.h"
#include "event/event_dispatcher.h"
#include "event/window_event.h"
#include "layer_stack.h"
#include "util.h"
#include "window.h"
#include <chrono>
#include <memory>
#include <ranges>

namespace engine_core {

#pragma warning(push)
#pragma warning(disable : 4251)

class ENGINE_CORE_API App {
protected:
	App() : m_layer_stack(this) {}

public:
	virtual ~App();

	void run();
	void close() { m_running = false; }

	/// Called once before the main loop. Override for app-level setup.
	virtual void on_init() {}

	/// Called every frame BEFORE layer updates.
	/// Override for per-frame app-level work (e.g. set clear color).
	virtual void on_update() {}

	/// Event hub: dispatches WindowCloseEvent → close(),
	/// then forwards remaining events to layers (reverse order, top-down).
	/// Override ONLY if you need custom event handling BEFORE layers.
	/// Call App::on_event(e) at the end to forward to layers.
	virtual void on_event(Event& e) {
		EventDispatcher d(e);
		d.dispatch<WindowCloseEvent>([this](auto&) {
			close();
			return true;
		});
		for (auto& layer : std::ranges::reverse_view(m_layer_stack)) {
			layer->on_event(e);
			if (e.is_handled())
				break;
		}
	}

	void push_layer(Layer* layer) { m_layer_stack.push_layer(layer); }
	void push_overlay(Layer* overlay) { m_layer_stack.push_overlay(overlay); }

	Window& get_window() { return *m_window; }

private:
	bool m_running = true;
	std::unique_ptr<Window> m_window;
	LayerStack m_layer_stack;
	std::chrono::steady_clock::time_point m_last_frame_time{};
};

#pragma warning(pop)

} // namespace engine_core
