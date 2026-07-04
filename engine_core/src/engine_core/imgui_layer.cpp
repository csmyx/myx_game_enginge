#include "imgui_layer.h"

#include <glad/glad.h>

#include "log.h"
#include "window.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

namespace engine_core {

ImGuiLayer::ImGuiLayer(App& app)
    : Layer(app, "ImGuiLayer")
{
}

ImGuiLayer::~ImGuiLayer() = default;

void ImGuiLayer::on_attach()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    auto* window = static_cast<GLFWwindow*>(get_window().get_native_window());
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    // Auto-detect DPI scale from the window
    float xscale = 1.0f, yscale = 1.0f;
    glfwGetWindowContentScale(window, &xscale, &yscale);
    set_ui_scale(xscale);

    ENGINE_CORE_INFO("ImGuiLayer attached (DPI scale: {:.1f})", xscale);
}

void ImGuiLayer::on_detach()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    ENGINE_CORE_INFO("ImGuiLayer detached");
}

void ImGuiLayer::set_ui_scale(float scale)
{
    if (scale <= 0.0f)
        scale = 1.0f;
    ImGui::GetStyle().ScaleAllSizes(scale);
    ImGui::GetIO().FontGlobalScale = scale;
}

void ImGuiLayer::on_event(Event& e)
{
    if (!m_block_events)
        return;

    ImGuiIO& io = ImGui::GetIO();
    bool captured = io.WantCaptureMouse || io.WantCaptureKeyboard;
    if (captured) {
        e.set_handled(true);
    }
}

void ImGuiLayer::begin()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void ImGuiLayer::end()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

} // namespace engine_core
