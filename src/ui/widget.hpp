#pragma once

#include "../rect.hpp"
#include "../service_provider.hpp"
#include "layout.hpp"
#include <SDL.h>

namespace ui {

    enum class Capabilites { Focusable, Hoverable };

    struct Widget {
    private:
        Layout m_layout;

    public:
        explicit Widget(Layout layout) : m_layout{ layout } { }

        Widget(const Widget&) = delete;
        Widget(Widget&&) = delete;
        Widget& operator=(const Widget&) = delete;
        Widget& operator=(Widget&&) = delete;

        virtual ~Widget() = default;

        [[nodiscard]] Layout layout() const {
            return m_layout;
        }

        virtual void render(const ServiceProvider& service_provider) const = 0;
        [[nodiscard]] virtual bool handle_event(const SDL_Event& event, const Window* window) = 0;
    };
} // namespace ui
