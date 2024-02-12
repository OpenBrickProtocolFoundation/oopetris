#pragma once

#include <functional>
#include <spdlog/spdlog.h>
#include <utility>

#include "focusable.hpp"
#include "graphics/rect.hpp"
#include "graphics/renderer.hpp"
#include "graphics/text.hpp"
#include "hoverable.hpp"
#include "manager/resource_manager.hpp"
#include "platform/capabilities.hpp"
#include "ui/widget.hpp"


namespace ui {

    struct Button : public Widget, public Focusable, public Hoverable {
    public:
        using Callback = std::function<void(const Button&)>;

    private:
        std::string m_caption;
        Callback m_callback;
        std::pair<u32, u32> margin;
        Rect m_fill_rect;

        explicit Button(
                std::string caption,
                usize focus_id,
                Callback callback,
                std::pair<u32, u32> margin,
                const Rect& fill_rect,
                const Layout& layout
        )
            : Widget(layout),
              Focusable{ focus_id },
              Hoverable{ fill_rect },
              m_caption{ std::move(caption) },
              m_callback{ std::move(callback) },
              margin{ margin },
              m_fill_rect{ fill_rect } { }

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
            : Button{
                  std::move(caption),
                  focus_id,
                  std::move(callback),
                  {static_cast<u32>(margin.first * size.first), static_cast<u32>(margin.second * size.second)},
                  ui::get_rectangle_aligned(
                          layout,
                          static_cast<u32>(size.first * layout.get_rect().width()),
                          static_cast<u32>(size.second * layout.get_rect().height()),
                          alignment
                  ),
                  layout
        } { }


        void render(const ServiceProvider& service_provider) const override {
            const auto color =
                    (has_focus()    ? is_hovered() ? Color(0xFF, 0x6A, 0x00) : Color::red()
                        : is_hovered() ? Color(0x00, 0xBB, 0xFF)
                                    : Color::blue());
            service_provider.renderer().draw_rect_filled(m_fill_rect, color);

            const Rect text_area{ m_fill_rect.top_left.x + static_cast<int>(margin.first),
                                  m_fill_rect.top_left.y + static_cast<int>(margin.second),
                                  m_fill_rect.width() - 2 * static_cast<int>(margin.first),
                                  m_fill_rect.height() - 2 * static_cast<int>(margin.second) };
            service_provider.renderer().draw_text(
                    text_area, m_caption, service_provider.fonts().get(FontId::Default), Color::white()
            );
        }

        bool handle_event(const SDL_Event& event, const Window* window) override {

            if (utils::device_supports_keys()) {
                if (has_focus() and utils::event_is_action(event, utils::CrossPlatformAction::OK)) {
                    spdlog::info("button pressed");
                    on_clicked();
                    return true;
                }
            }

            if (detect_hover(event, window)) {
                return true;
            }

            return false;
        }


        void on_clicked() override {
            m_callback(*this);
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
