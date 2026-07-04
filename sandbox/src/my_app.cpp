#include "engine_core/event/mouse_event.h"
#include <GLFW/glfw3.h>
#include <engine_core.h>

class MyApp : public engine_core::App {
public:
  MyApp() { ENGINE_CLIENT_INFO("MyApp constructed."); }
  ~MyApp() { ENGINE_CLIENT_INFO("MyApp destructed."); }
  void on_event(engine_core::Event &event) override {
    static int event_count = 0;
    event_count++;
    ENGINE_CLIENT_INFO("MyApp received an event: {}, Count: {}",
                       event.get_name(), event_count);
  }
  void on_update() override {
    static int update_count = 0;
    update_count++;
    ENGINE_CLIENT_INFO("MyApp on_update() called. Frame: {}", update_count);
  }
};

class MyApp2 : public engine_core::App {
  bool m_wireframe = false;

  void on_event(engine_core::Event &e) override {
    engine_core::EventDispatcher d(e);
    d.dispatch<engine_core::WindowCloseEvent>([this](auto &) {
      close();
      return true;
    });
    d.dispatch<engine_core::WindowResizeEvent>([](auto &e) {
      ENGINE_CORE_INFO("Window resized to {}x{}", e.get_width(),
                       e.get_height());
      return false;
    });
    d.dispatch<engine_core::KeyPressedEvent>([this](auto &e) {
      if (e.get_key_code() == GLFW_KEY_ESCAPE) {
        close();
        return true;
      }
      if (e.get_key_code() == GLFW_KEY_F1) {
        m_wireframe = !m_wireframe;
        ENGINE_CORE_INFO("Wireframe mode: {}", m_wireframe ? "ON" : "OFF");
        return true;
      }
      return false;
    });
    d.dispatch<engine_core::MouseButtonReleasedEvent>([this](auto &e) {
      ENGINE_CORE_INFO("Mouse button released: {}", e.get_mouse_button());

      return true;
    });
  }

  void on_update() override {
    // 每帧逻辑
  }
};

engine_core::App *create_app() { return new MyApp2(); }
