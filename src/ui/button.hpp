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
#include "hoverable.hpp"
#include "widget.hpp"


namespace ui {

    struct Button : public Widget, public Focusable, public Hoverable {
    public:
        using Callback = std::function<void(const Button&)>;

    private:
        std::string m_caption;
        Callback m_callback;
        std::pair<u32, u32> margin;
        Rect fill_rect;

    public:
        explicit Button(
                std::string caption,
                usize focus_id,
                Callback callback,
                std::pair<double, double> size,
                Alignment alignment,
                std::pair<double, double> margin,
                const Layout& layout
        )
            : Widget(layout),
              Focusable{ focus_id },
              m_caption{ std::move(caption) },
              m_callback{ std::move(callback) },
              margin{ static_cast<u32>(margin.first * size.first), static_cast<u32>(margin.second * size.second) },
              fill_rect{ ui::get_rectangle_aligned(
                      layout,
                      static_cast<u32>(size.first * layout.get_rect().width()),
                      static_cast<u32>(size.second * layout.get_rect().height()),
                      alignment
              ) } { }


        void render(const ServiceProvider& service_provider) const override {
            const auto color =
                    (has_focus()    ? is_hovered() ? Color(0xFF, 0x6A, 0x00) : Color::red()
                        : is_hovered() ? Color(0x00, 0xBB, 0xFF)
                                    : Color::blue());
            service_provider.renderer().draw_rect_filled(fill_rect, color);

            const Rect text_area{ fill_rect.top_left.x + static_cast<int>(margin.first),
                                  fill_rect.top_left.y + static_cast<int>(margin.second),
                                  fill_rect.width() - 2 * static_cast<int>(margin.first),
                                  fill_rect.height() - 2 * static_cast<int>(margin.second) };
            service_provider.renderer().draw_text(
                    text_area, m_caption, service_provider.fonts().get(FontId::Default), Color::white()
            );
        }

        bool handle_event(const SDL_Event& event, const Window* window) override {

            if (utils::device_supports_keys()) {
                if (has_focus() and utils::event_is_action(event, utils::CrossPlatformAction::OK)) {
                    spdlog::info("button pressed");
                    m_callback(*this);
                    return true;
                }
            }


            if (utils::device_supports_clicks()) {

                if (utils::event_is_click_event(event, utils::CrossPlatformClickEvent::Any)) {

                    if (utils::is_event_in(window, event, fill_rect)) {

                        on_hover();

                        if (utils::event_is_click_event(event, utils::CrossPlatformClickEvent::ButtonDown)) {
                            spdlog::info("button clicked");
                            m_callback(*this);
                        }

                        return true;

                    } else {
                        on_unhover();
                    }
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
