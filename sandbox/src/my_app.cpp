#include <engine_core.h>

// ── Layer pattern: modular game logic ──────────────────────

class SandboxLayer : public engine_core::Layer {
public:
  SandboxLayer() : Layer("SandboxLayer") {}

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
    // static int frame = 0;
    // if (++frame % 480 == 0) { // log every 60 frames
    //   ENGINE_CLIENT_INFO("SandboxLayer frame {}, dt={:.4f}s", frame, dt);
    // }
    // 静态变量：只会初始化一次
    static double last_print_time = glfwGetTime();
    double current_time = glfwGetTime();

    // 间隔 >= 1 秒打印一次
    if (current_time - last_print_time >= 1.0) {
      last_print_time = current_time;
      // 帧率 = 1 / dt
      float fps = 1.0f / dt;
      ENGINE_CLIENT_INFO("SandboxLayer dt={:.4f}s, FPS={:.1f}", dt, fps);
    }
  }
};

// ── App pattern: app-level initialization ──────────────────

class SandboxApp : public engine_core::App {
  void on_init() override {
    ENGINE_CLIENT_INFO("SandboxApp initializing...");
  }
};

// ── Entry point ────────────────────────────────────────────

engine_core::App *create_app() {
  auto *app = new SandboxApp();
  app->push_layer(new SandboxLayer());
  return app;
}
