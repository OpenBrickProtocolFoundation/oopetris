#pragma once

#include <functional>
#include <spdlog/spdlog.h>
#include <type_traits>
#include <utility>

#include "graphics/rect.hpp"
#include "input/input.hpp"
#include "ui/focusable.hpp"
#include "ui/widget.hpp"

namespace ui {

    template<typename Type>
    struct AbstractSlider : public Widget, public Focusable {
    public:
        using Range = std::pair<Type, Type>;
        using Getter = std::function<Type()>;
        using Setter = std::function<void(Type)>;

    private:
        Range m_range;
        Getter m_getter;
        Setter m_setter;
        Type m_step;
        Type m_current_value;
        bool m_is_dragging{ false };
        shapes::URect m_bar_rect;
        shapes::URect m_slider_rect;

        [[nodiscard]] virtual std::pair<shapes::URect, shapes::URect> get_rectangles() const = 0;

    protected:
        void change_layout() {

            const auto& [bar_rect, slider_rect] = this->get_rectangles();

            m_bar_rect = bar_rect;
            m_slider_rect = slider_rect;
        }

        [[nodiscard]] const Range& range() const {
            return m_range;
        }

        [[nodiscard]] const Type& current_value() const {
            return m_current_value;
        }

        [[nodiscard]] bool is_dragging() const {
            return m_is_dragging;
        }

        [[nodiscard]] const shapes::URect& bar_rect() const {
            return m_bar_rect;
        }

        [[nodiscard]] const shapes::URect& slider_rect() const {
            return m_slider_rect;
        }

    public:
        explicit AbstractSlider(
                u32 focus_id,
                Range range,
                Getter getter,
                Setter setter,
                Type step,
                const Layout& layout,
                bool is_top_level
        )
            : Widget{ layout, WidgetType::Component, is_top_level },
              Focusable{ focus_id },
              m_range{ std::move(range) },
              m_getter{ std::move(getter) },
              m_setter{ std::move(setter) },
              m_step{ step } {
            assert(m_range.first <= m_range.second && "Range has to be in correct order!");
            m_current_value = m_getter();

            // if on top. we give us focus automatically
            if (is_top_level) {
                focus();
            }
        }


        ~AbstractSlider() override {
            SDL_CaptureMouse(SDL_FALSE);
        }


        Widget::EventHandleResult handle_event(
                const std::shared_ptr<input::InputManager>& input_manager,
                const SDL_Event& event
        ) // NOLINT(readability-function-cognitive-complexity)
                override {
            Widget::EventHandleResult handled = false;

            const auto navigation_event = input_manager->get_navigation_event(event);

            if (navigation_event.has_value() and has_focus()) {
                if (navigation_event == input::NavigationEvent::RIGHT) {
                    m_current_value = m_current_value + m_step;
                    if (m_current_value >= m_range.second) {
                        m_current_value = m_range.second;
                    }

                    handled = true;
                } else if (navigation_event == input::NavigationEvent::LEFT) {
                    m_current_value = m_current_value - m_step;
                    if (m_current_value <= m_range.first) {
                        m_current_value = m_range.first;
                    }

                    handled = true;
                }
            }

            const auto pointer_event = input_manager->get_pointer_event(event);


            if (not handled and pointer_event.has_value()) {

                const auto change_value_on_scroll = [&pointer_event, this]() {
                    const auto& [bar_rect, slider_rect] = this->get_rectangles();

                    const auto& [x, _] = pointer_event->position();

                    if (x <= static_cast<i32>(bar_rect.top_left.x)) {
                        m_current_value = m_range.first;
                    } else if (x >= static_cast<i32>(bar_rect.bottom_right.x)) {
                        m_current_value = m_range.second;
                    } else {

                        const Type percentage =
                                static_cast<Type>(x - bar_rect.top_left.x) / static_cast<Type>(bar_rect.width());
                        m_current_value = percentage * (m_range.second - m_range.first) + m_range.first;
                    }
                };


                if (pointer_event == input::PointerEvent::PointerDown) {

                    if (pointer_event->is_in(m_bar_rect)) {

                        change_value_on_scroll();
                        m_is_dragging = true;
                        SDL_CaptureMouse(SDL_TRUE);
                        handled = {
                            true,
                            { ui::EventHandleType::RequestFocus, this }
                        };

                    } else if (pointer_event->is_in(m_slider_rect)) {
                        m_is_dragging = true;
                        SDL_CaptureMouse(SDL_TRUE);
                        handled = {
                            true,
                            { ui::EventHandleType::RequestFocus, this }
                        };
                    }

                } else if (pointer_event == input::PointerEvent::PointerUp) {
                    // only handle this, if already dragging, otherwise it's a button down from previously or some other widget
                    if (m_is_dragging) {
                        m_is_dragging = false;
                        SDL_CaptureMouse(SDL_FALSE);
                        handled = true;
                    }
                } else if (pointer_event == input::PointerEvent::Motion) {

                    if (m_is_dragging) {
                        change_value_on_scroll();
                        handled = true;
                    }

                    //TODO: this is not working, since pointer_event.has_value() is wrong in this case
                } else if (event.type == SDL_MOUSEWHEEL && has_focus()) {

                    // here we use a reverse scroll behaviour, since moving the mouse up is always considered increasing the volume, regardless of you OS setting about natural scrolling or not
                    const bool direction_is_up =
                            event.wheel.direction == SDL_MOUSEWHEEL_NORMAL ? event.wheel.y > 0 : event.wheel.y < 0;

                    if (direction_is_up) {
                        m_current_value = m_current_value + m_step;
                        if (m_current_value >= m_range.second) {
                            m_current_value = m_range.second;
                        }

                    } else {
                        m_current_value = m_current_value - m_step;
                        if (m_current_value <= m_range.first) {
                            m_current_value = m_range.first;
                        }
                    }

                    handled = true;
                }
            }


            if (handled) {
                m_setter(m_current_value);
                change_layout();
                return handled;
            }

            return false;
        }

        void on_change() {
            m_current_value = m_getter();
            change_layout();
        }
    };

} // namespace ui
