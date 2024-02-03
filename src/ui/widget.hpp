#pragma once

#include "../rect.hpp"
#include "../service_provider.hpp"
#include "layout.hpp"
#include <SDL.h>

namespace ui {

    enum class Capabilites { Focusable, Hoverable };

    struct Widget {
        Layout layout;

        explicit Widget(Layout layout) : layout{ layout } { }

        Widget(const Widget&) = delete;
        Widget(Widget&&) = delete;
        Widget& operator=(const Widget&) = delete;
        Widget& operator=(Widget&&) = delete;

        virtual ~Widget() = default;

        virtual void render(const ServiceProvider& service_provider) const = 0;
        [[nodiscard]] virtual bool handle_event(const SDL_Event& event, const Window* window) = 0;
    };
} // namespace ui
