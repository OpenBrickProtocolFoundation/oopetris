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

    struct Slider : public Widget, public Focusable {
    public:
        using Range = std::pair<float, float>;
        using Getter = std::function<float()>;
        using Setter = std::function<void(const float&)>;


    private:
        std::string m_caption;
        Range m_range;
        Getter m_getter;
        Setter m_setter;
        float m_step;
        Window* m_window;
        float current_value;

        [[nodiscard]] std::pair<Point, Rect> get_fill_rect(const Rect screen_rect) const {
            const auto absolute_layout = std::get<AbsoluteLayout>(layout);
            const auto origin = Point{ static_cast<int>(absolute_layout.x), static_cast<int>(absolute_layout.y) }
                                + screen_rect.top_left;
            return {
                origin, Rect{origin + Point{ 0, 40 }, origin + Point{ 400, 45 }}
            };
        }

    public:
        explicit Slider(
                std::string caption,
                const Layout& layout,
                usize focus_id,
                const Range& range,
                const Getter& getter,
                const Setter& setter,
                float step,
                Window* window
        )
            : Widget(layout),
              Focusable{ focus_id },
              m_caption{ std::move(caption) },
              m_range{ range },
              m_getter{ getter },
              m_setter{ setter },
              m_step{ step },
              m_window{ window } {
            assert(m_range.first <= m_range.second && "Range has to be in correct order!");
            current_value = m_getter();
        }


        void render(const ServiceProvider& service_provider, const Rect screen_rect) const override {
            const auto color = (has_focus() ? Color::red() : Color::blue());
            const auto [origin, fill_area] = get_fill_rect(screen_rect);
            service_provider.renderer().draw_rect_filled(fill_area, color);
            service_provider.renderer().draw_text(
                    origin, m_caption, service_provider.fonts().get(FontId::Default), Color::white()
            );

            float percentage = (current_value - m_range.first) / (m_range.second - m_range.first);

            int position_x_middle = origin.x + static_cast<int>(percentage * (fill_area.bottom_right.x - origin.x));

            const auto slider_rect = Rect{
                Point{position_x_middle - 5,     fill_area.top_left.y - 20},
                Point{position_x_middle + 5, fill_area.bottom_right.y + 20}
            };
            //orange or cyan
            const auto slider_color = (has_focus() ? Color(255, 111, 0) : Color(0, 204, 255));
            service_provider.renderer().draw_rect_filled(slider_rect, slider_color);
        }

        bool handle_event(const SDL_Event& event) override {
            //TODO: handle mouse events (dragging and clicking)
            bool changed = false;
            if (utils::event_is_action(event, utils::CrossPlatformAction::RIGHT)) {
                current_value = current_value + m_step;
                if (current_value >= m_range.second) {
                    current_value = m_range.second;
                }

                changed = true;
            } else if (utils::event_is_action(event, utils::CrossPlatformAction::LEFT)) {
                current_value = current_value - m_step;
                if (current_value <= m_range.first) {
                    current_value = m_range.first;
                }

                changed = true;
            }


            if (changed) {
                m_setter(current_value);
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
