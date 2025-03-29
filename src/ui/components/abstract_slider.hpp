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
        bool m_mouse_captured{ false };

        [[nodiscard]] virtual std::pair<shapes::URect, shapes::URect> get_rectangles() const = 0;

        void capture_mouse(bool value) {
            assert(m_mouse_captured != value);
            m_mouse_captured = value;
            SDL_CaptureMouse(value ? SDL_TRUE : SDL_FALSE);
        }

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
            if (m_mouse_captured) {
                capture_mouse(false);
            }
        }


        AbstractSlider(const AbstractSlider&) = default;
        AbstractSlider& operator=(const AbstractSlider&) = default;

        AbstractSlider(AbstractSlider&&) noexcept = default;
        AbstractSlider& operator=(AbstractSlider&&) noexcept = default;

        [[nodiscard]] bool has_mouse_captured() {
            return m_mouse_captured;
        }


        Widget::EventHandleResult handle_event( // NOLINT(readability-function-cognitive-complexity)
                const std::shared_ptr<input::InputManager>& input_manager,
                const SDL_Event& event
        ) override {
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
                        capture_mouse(true);

                        handled = {
                            true,
                            { ui::EventHandleType::RequestFocus, this, nullptr }
                        };

                    } else if (pointer_event->is_in(m_slider_rect)) {

                        m_is_dragging = true;
                        capture_mouse(true);

                        handled = {
                            true,
                            { ui::EventHandleType::RequestFocus, this, nullptr }
                        };
                    }

                } else if (pointer_event == input::PointerEvent::PointerUp) {
                    // only handle this, if already dragging, otherwise it's a button down from previously or some other widget
                    if (m_is_dragging) {
                        m_is_dragging = false;
                        capture_mouse(false);
                        handled = true;
                    }
                } else if (pointer_event == input::PointerEvent::Motion) {

                    if (m_is_dragging) {
                        change_value_on_scroll();
                        handled = true;
                    }
                } else if (has_focus() and pointer_event == input::PointerEvent::Wheel) {

                    if (pointer_event->is_in(layout().get_rect())) {

                        // if we should support more in teh future, we would have to abstract this better ways, since  accessing event.wheel is not abstracted away atm
                        if (event.type == SDL_MOUSEWHEEL) {

                            // here we use a reverse scroll behaviour, since moving the mouse up is always considered increasing the volume, regardless of you OS setting about natural scrolling or not
                            const bool direction_is_up = event.wheel.direction == SDL_MOUSEWHEEL_NORMAL
                                                                 ? event.wheel.y > 0
                                                                 : event.wheel.y < 0;

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
