
#if defined(__CONSOLE__)
#include "console_input.hpp"


void JoystickInput::handle_event(const SDL_Event& event, const Window*) {
    m_event_buffer.push_back(event);
}

void JoystickInput::update(SimulationStep simulation_step_index) {
    for (const auto& event : m_event_buffer) {
        const auto input_event = sdl_event_to_input_event(event);
        if (input_event.has_value()) {
            Input::handle_event(*input_event, simulation_step_index);
        }
    }
    m_event_buffer.clear();

    Input::update(simulation_step_index);
}

#if defined(__SWITCH__)


helper::optional<InputEvent> JoystickInput::sdl_event_to_input_event(const SDL_Event& event) const {
    if (event.type == SDL_JOYBUTTONDOWN) {
        const auto button = event.jbutton.button;
        if (button == JOYCON_CROSS_LEFT) {
            return InputEvent::RotateLeftPressed;
        }
        if (button == JOYCON_CROSS_RIGHT) {
            return InputEvent::RotateRightPressed;
        }
        if (button == JOYCON_LDPAD_DOWN or button == JOYCON_RDPAD_DOWN) {
            return InputEvent::MoveDownPressed;
        }
        if (button == JOYCON_LDPAD_LEFT or button == JOYCON_RDPAD_LEFT) {
            return InputEvent::MoveLeftPressed;
        }
        if (button == JOYCON_LDPAD_RIGHT or button == JOYCON_RDPAD_RIGHT) {
            return InputEvent::MoveRightPressed;
        }
        if (button == JOYCON_X) {
            return InputEvent::DropPressed;
        }
        if (button == JOYCON_B) {
            return InputEvent::HoldPressed;
        }
    } else if (event.type == SDL_JOYBUTTONUP) {
        const auto button = event.jbutton.button;
        if (button == JOYCON_CROSS_LEFT) {
            return InputEvent::RotateLeftReleased;
        }
        if (button == JOYCON_CROSS_RIGHT) {
            return InputEvent::RotateRightReleased;
        }
        if (button == JOYCON_LDPAD_DOWN or button == JOYCON_RDPAD_DOWN) {
            return InputEvent::MoveDownReleased;
        }
        if (button == JOYCON_LDPAD_LEFT or button == JOYCON_RDPAD_LEFT) {
            return InputEvent::MoveLeftReleased;
        }
        if (button == JOYCON_LDPAD_RIGHT or button == JOYCON_RDPAD_RIGHT) {
            return InputEvent::MoveRightReleased;
        }
        if (button == JOYCON_X) {
            return InputEvent::DropReleased;
        }
        if (button == JOYCON_B) {
            return InputEvent::HoldReleased;
        }
    }
    return helper::nullopt;
}
#elif defined(__3DS__)


helper::optional<InputEvent> JoystickInput::sdl_event_to_input_event(const SDL_Event& event) const {
    if (event.type == SDL_JOYBUTTONDOWN) {
        const auto button = event.jbutton.button;
        if (button == JOYCON_L) {
            return InputEvent::RotateLeftPressed;
        }
        if (button == JOYCON_R) {
            return InputEvent::RotateRightPressed;
        }
        if (button == JOYCON_DPAD_DOWN or button == JOYCON_CSTICK_DOWN) {
            return InputEvent::MoveDownPressed;
        }
        if (button == JOYCON_DPAD_LEFT or button == JOYCON_CSTICK_LEFT) {
            return InputEvent::MoveLeftPressed;
        }
        if (button == JOYCON_DPAD_RIGHT or button == JOYCON_CSTICK_RIGHT) {
            return InputEvent::MoveRightPressed;
        }
        if (button == JOYCON_A) {
            return InputEvent::DropPressed;
        }
        if (button == JOYCON_B) {
            return InputEvent::HoldPressed;
        }
    } else if (event.type == SDL_JOYBUTTONUP) {
        const auto button = event.jbutton.button;
        if (button == JOYCON_L) {
            return InputEvent::RotateLeftReleased;
        }
        if (button == JOYCON_R) {
            return InputEvent::RotateRightReleased;
        }
        if (button == JOYCON_DPAD_DOWN or button == JOYCON_CSTICK_DOWN) {
            return InputEvent::MoveDownReleased;
        }
        if (button == JOYCON_DPAD_LEFT or button == JOYCON_CSTICK_LEFT) {
            return InputEvent::MoveLeftReleased;
        }
        if (button == JOYCON_DPAD_RIGHT or button == JOYCON_CSTICK_RIGHT) {
            return InputEvent::MoveRightReleased;
        }
        if (button == JOYCON_A) {
            return InputEvent::DropReleased;
        }
        if (button == JOYCON_B) {
            return InputEvent::HoldReleased;
        }
    }
    return helper::nullopt;
}
#endif


#endif
