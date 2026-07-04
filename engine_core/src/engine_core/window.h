#pragma once

#include <functional>
#include <string>

#include "event/event.h"
#include "util.h"

namespace engine_core {

struct WindowProps {
    std::string title;
    unsigned int width;
    unsigned int height;

    WindowProps(const std::string& t = "Myx Engine", unsigned int w = 1280,
        unsigned int h = 720)
        : title(t)
        , width(w)
        , height(h)
    {
    }
};

class ENGINE_CORE_API Window {
public:
    virtual ~Window() = default;

    virtual void on_update() = 0;
    virtual unsigned int get_width() const = 0;
    virtual unsigned int get_height() const = 0;
    virtual bool should_close() const = 0;
    virtual void set_event_callback(
        const std::function<void(Event&)>& callback) = 0;
    virtual void set_vsync(bool enabled) = 0;
    virtual bool is_vsync() const = 0;
    virtual void set_clear_color(float r, float g, float b, float a) = 0;
    virtual void clear() = 0;
    virtual void swap_buffers() = 0;
    virtual void* get_native_window() const = 0;

    static Window* create(const WindowProps& props = WindowProps());
};

} // namespace engine_core
