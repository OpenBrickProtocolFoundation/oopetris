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

    struct Button : public Widget, public Focusable /* , public Hoverable */ {
    public:
        using Callback = std::function<void(const Button&)>;

    private:
        std::string m_caption;
        Callback m_callback;
        std::pair<u32, u32> m_size;
        Alignment m_alignment;
        std::pair<u32, u32> margin;

        [[nodiscard]] inline Rect get_fill_rect() const {
            return ui::get_rectangle_aligned(layout, m_size.first, m_size.second, m_alignment);
        }

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
              m_size{ static_cast<u32>(size.first * layout.get_rect().width()),
                      static_cast<u32>(size.second * layout.get_rect().height()) },
              m_alignment{ alignment },
              margin{ static_cast<u32>(margin.first * m_size.first), static_cast<u32>(margin.second * m_size.second) } {
        }


        void render(const ServiceProvider& service_provider) const override {
            //TODO
            const auto is_hovered = true;
            const auto color = (has_focus() ? Color::red() : is_hovered ? Color(0, 0xBB, 0xFF) : Color::blue());
            const auto fill_area = get_fill_rect();
            service_provider.renderer().draw_rect_filled(fill_area, color);

            const Rect text_area{ fill_area.top_left.x + static_cast<int>(margin.first),
                                  fill_area.top_left.y + static_cast<int>(margin.second),
                                  fill_area.width() - 2 * static_cast<int>(margin.first),
                                  fill_area.height() - 2 * static_cast<int>(margin.second) };
            service_provider.renderer().draw_text(
                    text_area, m_caption, service_provider.fonts().get(FontId::Default), Color::white()
            );
        }

        bool handle_event(const SDL_Event& event) override {
            // attention don't combine this without ifdefs, since an SDL_MOUSEBUTTONDOWN may contain event.which == SDL_TOUCH_MOUSEID which means SDL made a mouse event up from a touch!


            /*  if (is_hovered() && utils::is_clicked(event, ClickType::OnButtonDown)) {
                spdlog::info("button clicked");
                m_callback(*this);
                return true;
            } */


            if (utils::device_supports_keys()) {
                if (has_focus() and utils::event_is_action(event, utils::CrossPlatformAction::OK)) {
                    spdlog::info("button pressed");
                    m_callback(*this);
                    return true;
                }
            }
            return false;
        }

        [[nodiscard]] std::vector<Capabilites> get_capabilities() const override {
            return { Capabilites::Focusable, Capabilites::Hoverable };
        }

    private:
        void on_focus() override {
            spdlog::info("button focused");
        }

        void on_unfocus() override {
            spdlog::info("button unfocused");
        }

        /* void on_hover() override {
            spdlog::info("button hovered");
        }

        void on_unhover() override {
            spdlog::info("button un-hovered");
        } */
    };

} // namespace ui
