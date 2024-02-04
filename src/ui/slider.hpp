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
        Range m_range;
        Getter m_getter;
        Setter m_setter;
        float m_step;
        float current_value;
        bool is_dragging;
        Rect fill_rect;

        [[nodiscard]] std::pair<Rect, Rect> get_rectangles() const {

            const auto origin = fill_rect.top_left;

            const auto bar = RelativeLayout{ fill_rect, 0, 0.4, 1.0, 0.2 };

            const float percentage = (current_value - m_range.first) / (m_range.second - m_range.first);

            const int position_x_middle =
                    origin.x + static_cast<int>(percentage * static_cast<float>(fill_rect.bottom_right.x - origin.x));

            const auto slider_rect = Rect{
                Point{position_x_middle - 5,     fill_rect.top_left.y},
                Point{position_x_middle + 5, fill_rect.bottom_right.y}
            };

            return { bar.get_rect(), slider_rect };
        }


    public:
        explicit Slider(
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
              m_range{ range },
              m_getter{ getter },
              m_setter{ setter },
              m_step{ step },
              fill_rect{ ui::get_rectangle_aligned(
                      layout,
                      static_cast<u32>(size.first * layout.get_rect().width()),
                      static_cast<u32>(size.second * layout.get_rect().height()),
                      alignment
              ) } {
            assert(m_range.first <= m_range.second && "Range has to be in correct order!");
            current_value = m_getter();
        }


        void render(const ServiceProvider& service_provider) const override {
            const auto color = (has_focus() ? Color::red() : Color::blue());

            const auto& [bar_rect, slider_rect] = get_rectangles();

            service_provider.renderer().draw_rect_filled(bar_rect, color);

            //orange or cyan
            const auto slider_color =
                    (is_dragging   ? Color(0xFF, 0xCC, 0x00)
                     : has_focus() ? Color(255, 111, 0)
                                   : Color(0, 204, 255));
            service_provider.renderer().draw_rect_filled(slider_rect, slider_color);
        }

        bool handle_event(const SDL_Event& event, const Window* window) override {
            if (not has_focus()) {
                return false;
            }

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

            if (not changed and utils::device_supports_clicks()) {

                if (utils::event_is_click_event(event, utils::CrossPlatformClickEvent::ButtonDown)) {
                    const auto& [_, slider_rect] = get_rectangles();
                    if (utils::is_event_in(window, event, slider_rect)) {
                        is_dragging = true;
                        changed = true;
                    }

                } else if (utils::event_is_click_event(event, utils::CrossPlatformClickEvent::ButtonUp)) {
                    is_dragging = false;
                    changed = true;

                } else if (utils::event_is_click_event(event, utils::CrossPlatformClickEvent::Motion)) {

                    if (is_dragging) {
                        const auto& [bar_rect, _] = get_rectangles();

                        const auto& [x, _1] = utils::get_raw_coordinates(window, event);

                        if (x <= bar_rect.top_left.x) {
                            current_value = m_range.first;
                        } else if (x >= bar_rect.bottom_right.x) {
                            current_value = m_range.second;
                        } else {

                            const float percentage = (x - bar_rect.top_left.x) / static_cast<float>(bar_rect.width());
                            current_value = percentage * (m_range.second - m_range.first) + m_range.first;
                        }

                        changed = true;
                    }
                } else if (event.type == SDL_MOUSEWHEEL) {

                    const bool direction_is_up =
                            event.wheel.direction == SDL_MOUSEWHEEL_NORMAL ? event.wheel.y > 0 : event.wheel.y < 0;

                    if (direction_is_up) {
                        current_value = current_value + m_step;
                        if (current_value >= m_range.second) {
                            current_value = m_range.second;
                        }

                        changed = true;
                    } else {
                        current_value = current_value - m_step;
                        if (current_value <= m_range.first) {
                            current_value = m_range.first;
                        }

                        changed = true;
                    }
                }
            }


            if (changed) {
                m_setter(current_value);
                return true;
            }

            return false;
        }

        void on_change() {
            current_value = m_getter();
            m_setter(current_value);
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
