#include "layer.h"
#include "app.h"

namespace engine_core {

Layer::~Layer() {}

void Layer::on_imgui_render() {}

App& Layer::get_app() {
	return *m_app;
}

Window& Layer::get_window() {
	return m_app->get_window();
}

} // namespace engine_core