#pragma once

#include "event/event.h"
#include "util.h"

namespace engine_core {

#pragma warning(push)
#pragma warning(disable : 4251)
class ENGINE_CORE_API Layer {
public:
  Layer(const std::string &name = "Layer") : m_debug_name(name) {};
  virtual ~Layer();

  virtual void on_attach() {}
  virtual void on_detach() {}
  virtual void on_update(float delta_time) {}
  virtual void on_event(Event &e) {}
  virtual void on_imgui_render();

  const std::string &get_name() const { return m_debug_name; }

protected:
  std::string m_debug_name;

private:
};
} // namespace engine_core

#pragma warning(pop)