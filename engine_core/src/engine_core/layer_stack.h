#pragma once

#include <vector>

#include "event/event.h"
#include "layer.h"

namespace engine_core {

class LayerStack {
public:
    LayerStack() = default;
    ~LayerStack()
    {
        for (auto layer : m_layers) {
            layer->on_detach();
            delete layer;
        }
    }

    void push_layer(Layer* layer)
    {
        m_layers.insert(m_layers.begin() + m_layer_insert_index, layer);
        m_layer_insert_index++;
    }
    void push_overlay(Layer* overlay) { m_layers.push_back(overlay); }

    /// Call after window is ready. Triggers on_attach() on all layers.
    void attach_all()
    {
        for (auto layer : m_layers)
            layer->on_attach();
    }

    void on_update(float delta_time)
    {
        // 正序更新：底层→顶层
        for (auto layer : m_layers)
            layer->on_update(delta_time);
    }

    void on_imgui_render()
    {
        // 正序渲染：先场景，后UI
        for (auto layer : m_layers)
            layer->on_imgui_render();
    }

    void on_event(Event& e)
    {
        // 倒序分发：顶层Overlay优先处理
        for (auto it = m_layers.rbegin(); it != m_layers.rend(); ++it) {
            (*it)->on_event(e);
            if (e.is_handled())
                break; // 事件被消费，直接终止
        }
    }
    size_t size() const { return m_layers.size(); }

public:
    using iterator = typename std::vector<Layer*>::iterator;
    using const_iterator = typename std::vector<Layer*>::const_iterator;

    iterator begin() noexcept { return m_layers.begin(); }
    iterator end() noexcept { return m_layers.end(); }
    const_iterator cbegin() const noexcept { return m_layers.cbegin(); }
    const_iterator cend() const noexcept { return m_layers.cend(); }

    using reverse_iterator = typename std::vector<Layer*>::reverse_iterator;
    using const_reverse_iterator =
        typename std::vector<Layer*>::const_reverse_iterator;
    reverse_iterator rbegin() noexcept { return m_layers.rbegin(); }
    reverse_iterator rend() noexcept { return m_layers.rend(); }
    const_reverse_iterator crbegin() const noexcept { return m_layers.crbegin(); }
    const_reverse_iterator crend() const noexcept { return m_layers.crend(); }

private:
    std::vector<Layer*> m_layers;
    unsigned int m_layer_insert_index = 0;
};
} // namespace engine_core