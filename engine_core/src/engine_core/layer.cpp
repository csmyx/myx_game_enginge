#include "layer.h"
#include "app.h"

namespace engine_core {

Layer::Layer(App& app, const std::string& name)
	: m_debug_name(name), m_app(&app) {}

Layer::~Layer() {}

void Layer::on_imgui_render() {}

Window& Layer::get_window() {
	return m_app->get_window();
}

} // namespace engine_core