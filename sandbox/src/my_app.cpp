#include <engine_core.h>

class MyLayer1 : public engine_core::Layer {
public:
  void on_event(engine_core::Event &event) override {
    static int event_count = 0;
    event_count++;
    ENGINE_CLIENT_INFO("Layer1 received event: {}, Count: {}",
                       event.get_name(), event_count);
  }
  void on_update(float delta_time) override {
    static int update_count = 0;
    update_count++;
    ENGINE_CLIENT_INFO("Layer1 on_update() Frame: {}", update_count);
  }
};

class MyApp2 : public engine_core::App {
  void on_event(engine_core::Event &e) override {
    engine_core::EventDispatcher d(e);
    d.dispatch<engine_core::WindowCloseEvent>([this](auto &) {
      close();
      return true;
    });
    d.dispatch<engine_core::KeyPressedEvent>([this](auto &e) {
      if (e.get_key_code() == GLFW_KEY_ESCAPE) {
        close();
        return true;
      }
      return false;
    });
    d.dispatch<engine_core::MouseButtonReleasedEvent>([](auto &e) {
      ENGINE_CLIENT_INFO("Mouse button released: {}", e.get_mouse_button());
      return true;
    });
  }

  void on_update() override {
    get_window().set_clear_color(0.2f, 0.3f, 0.4f, 1.0f);
  }
};

engine_core::App *create_app() {
  auto *app = new MyApp2();
  app->push_layer(new MyLayer1());
  return app;
}
