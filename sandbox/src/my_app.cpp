#include <engine_core.h>

// ── Layer pattern: modular game logic ──────────────────────

class SandboxLayer : public engine_core::Layer {
public:
  SandboxLayer(engine_core::App &app) : Layer(app, "SandboxLayer") {}

  void on_attach() override {
    ENGINE_CLIENT_INFO("SandboxLayer attached");
    // Layer can access the Window via get_window()
    get_window().set_clear_color(0.2f, 0.3f, 0.4f, 1.0f);
  }

  void on_event(engine_core::Event &e) override {
    engine_core::EventDispatcher d(e);
    d.dispatch<engine_core::KeyPressedEvent>([](auto &e) {
      if (e.get_key_code() == GLFW_KEY_ESCAPE) {
        ENGINE_CLIENT_INFO("ESC pressed — use WindowCloseEvent to quit");
      }
      return false; // don't consume, let App base handle close
    });
    d.dispatch<engine_core::MouseButtonReleasedEvent>([this](auto &e) {
      ENGINE_CLIENT_INFO("Mouse button {} released", e.get_mouse_button());
      auto random_color = []() {
        return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
      };
      get_window().set_clear_color(random_color(), random_color(),
                                   random_color(), 1.0f);
      return true;
    });
  }

  void on_update(float dt) override {
    // per-frame logic here
  }

  void on_imgui_render() override {
    ImGui::Begin("Sandbox Debug");
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    ImGui::Text("Frame time: %.3f ms", 1000.0f / ImGui::GetIO().Framerate);
    ImGui::End();
  }
};

// ── App pattern: app-level initialization ──────────────────

class SandboxApp : public engine_core::App {
  void on_init() override { ENGINE_CLIENT_INFO("SandboxApp initializing..."); }
};

// ── Entry point ────────────────────────────────────────────

engine_core::App *create_app() {
  auto *app = new SandboxApp();
  app->push_layer(new SandboxLayer(*app));
  return app;
}
