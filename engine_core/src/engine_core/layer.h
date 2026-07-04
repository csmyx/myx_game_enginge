#pragma once

#include "event/event.h"
#include "util.h"
#include "window.h"

namespace engine_core {

class App; // forward decl — allow Layer to access App without full include

#pragma warning(push)
#pragma warning(disable : 4251)
class ENGINE_CORE_API Layer {
	friend class LayerStack;

public:
	Layer(const std::string& name = "Layer") : m_debug_name(name) {}
	virtual ~Layer();

	virtual void on_attach() {}
	virtual void on_detach() {}
	virtual void on_update(float delta_time) {}
	virtual void on_event(Event& e) {}
	virtual void on_imgui_render();

	const std::string& get_name() const { return m_debug_name; }

protected:
	App& get_app();
	Window& get_window();

private:
	std::string m_debug_name;
	App* m_app = nullptr;
	void set_app(App* app) { m_app = app; }
};

#pragma warning(pop)

} // namespace engine_core