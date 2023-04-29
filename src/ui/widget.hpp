#pragma once

#include "../application.hpp"
#include "layout.hpp"
#include <SDL.h>

namespace ui {
    struct Widget {
        Layout layout;

        explicit Widget(Layout layout) : layout{ layout } { }
        virtual ~Widget() = default;

        virtual void render(const Application& app, Rect rect) const = 0;
        [[nodiscard]] virtual bool handle_event(const SDL_Event& event) = 0;
        virtual void on_resize_window(const Application& app) = 0;
    };
} // namespace ui
