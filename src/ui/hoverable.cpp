
#include "hoverable.hpp"

#include <iostream>

ui::Hoverable::Hoverable(const shapes::URect& fill_rect) : m_fill_rect{ fill_rect } { };

ui::Hoverable::~Hoverable() = default;

[[nodiscard]] bool ui::Hoverable::is_hovered() const {
    return m_is_hovered;
}

[[nodiscard]] const shapes::URect& ui::Hoverable::fill_rect() const {
    return m_fill_rect;
}

[[nodiscard]] helper::BoolWrapper<ui::ActionType>
ui::Hoverable::detect_hover(const std::shared_ptr<input::InputManager>& input_manager, const SDL_Event& event) {


    if (const auto result = input_manager->get_pointer_event(event); result.has_value()) {
        if (result->is_in(m_fill_rect)) {

            on_hover();

            switch (result->event()) {
                case input::PointerEvent::PointerDown:
                    return { true, ActionType::Clicked };
                case input::PointerEvent::PointerUp:
                    return { true, ActionType::Released };
                case input::PointerEvent::Motion:
                    return { true, ActionType::Hover };
                case input::PointerEvent::Wheel:
                    return false;

                default:
                    UNREACHABLE();
            }
        }

        on_unhover();
        return false;
    }

    return false;
}


void ui::Hoverable::on_hover() {
    m_is_hovered = true;
}

//TODO(Totto): this has to be used correctly, a click or focus change isn't an event, where an unhover needs to happen!
void ui::Hoverable::on_unhover() {
    m_is_hovered = false;
}
