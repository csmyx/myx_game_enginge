#include "imgui_layer.h"
#include "log.h"
#include "window.h"

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <imgui.h>
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

namespace engine_core {

ImGuiLayer::ImGuiLayer(App& app) : Layer(app, "ImGuiLayer") {}

ImGuiLayer::~ImGuiLayer() = default;

void ImGuiLayer::on_attach() {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	ImGui::StyleColorsDark();

	auto* window = static_cast<GLFWwindow*>(get_window().get_native_window());
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 460");

	ENGINE_CORE_INFO("ImGuiLayer attached");
}

void ImGuiLayer::on_detach() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	ENGINE_CORE_INFO("ImGuiLayer detached");
}

void ImGuiLayer::on_event(Event& e) {
	if (!m_block_events) return;

	ImGuiIO& io = ImGui::GetIO();
	bool captured = io.WantCaptureMouse || io.WantCaptureKeyboard;
	if (captured) {
		e.set_handled(true);
	}
}

void ImGuiLayer::begin() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void ImGuiLayer::end() {
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

} // namespace engine_core
