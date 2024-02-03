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
        float current_value;
        std::pair<u32, u32> m_size;
        Alignment m_alignment;

        [[nodiscard]] inline Rect get_fill_rect() const {
            return ui::get_rectangle_aligned(layout, m_size.first, m_size.second, m_alignment);
        }

    public:
        explicit Slider(
                std::string caption,
                usize focus_id,
                const Range& range,
                const Getter& getter,
                const Setter& setter,
                float step,
                std::pair<double, double> size,
                Alignment alignment,
                const Layout& layout
        )
            : Widget(layout),
              Focusable{ focus_id },
              m_caption{ std::move(caption) },
              m_range{ range },
              m_getter{ getter },
              m_setter{ setter },
              m_step{ step },
              m_size{ static_cast<u32>(size.first * layout.get_rect().width()),
                      static_cast<u32>(size.second * layout.get_rect().height()) },
              m_alignment{ alignment } {
            assert(m_range.first <= m_range.second && "Range has to be in correct order!");
            current_value = m_getter();
        }


        void render(const ServiceProvider& service_provider) const override {
            const auto color = (has_focus() ? Color::red() : Color::blue());
            const auto fill_area = get_fill_rect();
            const auto origin = fill_area.top_left;
            service_provider.renderer().draw_rect_filled(fill_area, color);
            service_provider.renderer().draw_text(
                    fill_area, m_caption, service_provider.fonts().get(FontId::Default), Color::white()
            );

            const float percentage = (current_value - m_range.first) / (m_range.second - m_range.first);

            const int position_x_middle =
                    origin.x + static_cast<int>(percentage * static_cast<float>(fill_area.bottom_right.x - origin.x));

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
    };

} // namespace ui
