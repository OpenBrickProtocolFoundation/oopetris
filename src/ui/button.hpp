#pragma once

#include <functional>
#include <spdlog/spdlog.h>
#include <utility>

#include "../capabilities.hpp"
#include "../rect.hpp"
#include "../renderer.hpp"
#include "../resource_manager.hpp"
#include "../text.hpp"
#include "focusable.hpp"
#include "widget.hpp"


namespace ui {

    struct Button : public Widget, public Focusable {
    public:
        using Callback = std::function<void(const Button&)>;

    private:
        std::string m_caption;
        Callback m_callback;
        Window* m_window;

        [[nodiscard]] std::pair<Point, Rect> get_fill_rect(const Rect screen_rect) const {
            const auto absolute_layout = std::get<AbsoluteLayout>(layout);
            const auto origin = Point{ static_cast<int>(absolute_layout.x), static_cast<int>(absolute_layout.y) }
                                + screen_rect.top_left;
            return {
                origin, Rect{origin, origin + Point{ 120, 40 }}
            };
        }

    public:
        explicit Button(std::string caption, const Layout& layout, usize focus_id, Callback callback, Window* window)
            : Widget(layout),
              Focusable{ focus_id },
              m_caption{ std::move(caption) },
              m_callback{ std::move(callback) },
              m_window{ window } { }


        void render(const ServiceProvider& service_provider, const Rect screen_rect) const override {
            const auto color = (has_focus() ? Color::red() : Color::blue());
            const auto [origin, fill_area] = get_fill_rect(screen_rect);
            service_provider.renderer().draw_rect_filled(fill_area, color);
            service_provider.renderer().draw_text(
                    origin, m_caption, service_provider.fonts().get(FontId::Default), Color::white()
            );
        }

        bool handle_event(const SDL_Event& event) override {

            // attention don't combine this without ifdefs, since an SDL_MOUSEBUTTONDOWN may contain event.which == SDL_TOUCH_MOUSEID which means SDL made a mouse event up from a touch!
            if (event.type ==
#if defined(__ANDROID__)
                //TODO: catch the release even otherwise an assertion fails!
                SDL_FINGERDOWN
#else
                        SDL_MOUSEBUTTONDOWN
                and event.button.button == SDL_BUTTON_LEFT
#endif
            ) {
#if defined(__ANDROID__)
                // These are doubles, from 0-1 in percent, the have to be casted to absolut x coordinates!
                const double x_percent = event.tfinger.x;
                const double y_percent = event.tfinger.y;
                const auto window_size = m_window->size();
                const auto x = static_cast<int>(std::round(x_percent * window_size.x));
                const auto y = static_cast<int>(std::round(y_percent * window_size.y));
#else
                const auto x = event.button.x;
                const auto y = event.button.y;
#endif

                const auto [_, fill_area] = get_fill_rect(m_window->screen_rect());

                const auto rect_start_x = fill_area.top_left.x;
                const auto rect_start_y = fill_area.top_left.y;
                const auto rect_end_x = fill_area.bottom_right.x;
                const auto rect_end_y = fill_area.bottom_right.y;


                const bool button_tapped =
                        (x >= rect_start_x and x <= rect_end_x and y >= rect_start_y and y <= rect_end_y);

                if (button_tapped) {
                    spdlog::info("button tapped");
                    m_callback(*this);
                    return true;
                }
            }


            if (utils::device_supports_keys()) {
                if (has_focus() and utils::event_is_action(event, utils::CrossPlatformAction::OK)) {
                    spdlog::info("button pressed");
                    m_callback(*this);
                    return true;
                }
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
