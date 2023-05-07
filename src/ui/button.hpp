#pragma once

#include "../rect.hpp"
#include "focusable.hpp"
#include "widget.hpp"
#include <functional>
#include <spdlog/spdlog.h>

namespace ui {

    struct Button : public Widget, public Focusable {
    public:
        using Callback = std::function<void(const Button&)>;

    private:
        Callback m_callback;

    public:
        explicit Button(const Layout& layout, usize focus_id, Callback callback)
            : Widget(layout),
              Focusable{ focus_id },
              m_callback{ std::move(callback) } { }

        void render(const ServiceProvider&, Rect) const override { }

        bool handle_event(const SDL_Event& event) override {
            // todo: create utils function for keydown test to prevent #ifdefs for android
            if (has_focus() and event.type == SDL_KEYDOWN and event.key.keysym.sym == SDLK_RETURN) {
                spdlog::info("button pressed");
                m_callback(*this);
                return true;
            }
            return false;
        }

    private:
        void on_focus() override {
            spdlog::info("button focused");
        }

        void on_unfocus() override {
            spdlog::info("button unfocused");
        }
    };

} // namespace ui
