#pragma once

#include "layer.h"

namespace engine_core {

/// Overlay layer that manages Dear ImGui context and rendering.
/// Push as overlay: app->push_overlay(new ImGuiLayer(app)).
/// Other layers draw their UI in on_imgui_render().
class ENGINE_CORE_API ImGuiLayer : public Layer {
public:
	explicit ImGuiLayer(App& app);
	~ImGuiLayer() override;

	void on_attach() override;
	void on_detach() override;
	void on_event(Event& e) override;

	/// Call before layers.on_imgui_render(): starts a new ImGui frame.
	void begin();
	/// Call after layers.on_imgui_render(): renders and draws ImGui data.
	void end();

	/// Scale all ImGui elements (fonts, spacing, etc.) by a factor.
	/// E.g. 2.0 for 200% high-DPI displays.
	void set_ui_scale(float scale);

private:
	bool m_block_events = true; // block mouse/keyboard events when ImGui wants them
};

} // namespace engine_core
