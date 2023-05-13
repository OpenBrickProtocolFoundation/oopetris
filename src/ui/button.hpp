#pragma once

#include <functional>
#include <spdlog/spdlog.h>

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

    public:
        explicit Button(std::string caption, const Layout& layout, usize focus_id, Callback callback)
            : Widget(layout),
              Focusable{ focus_id },
              m_caption{ std::move(caption) },
              m_callback{ std::move(callback) } { }

        void render(const ServiceProvider& service_provider, const Rect rect) const override {
            const auto color = (has_focus() ? Color::red() : Color::blue());
            const auto absolute_layout = std::get<AbsoluteLayout>(layout);
            const auto origin =
                    Point{ static_cast<int>(absolute_layout.x), static_cast<int>(absolute_layout.y) } + rect.top_left;
            const auto fill_area = Rect{
                origin, origin + Point{120, 40}
            };
            service_provider.renderer().draw_rect_filled(fill_area, color);
            service_provider.renderer().draw_text(
                    origin, m_caption, service_provider.fonts().get(FontId::Default), Color::white()
            );
        }

        bool handle_event(const SDL_Event& event) override {
            if (utils::device_supports_touch()) {
                //TODO how to handle focus on a touch device ???

                bool button_tapped = false;
                //TODO implement detection if the tap was inside the button!

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
