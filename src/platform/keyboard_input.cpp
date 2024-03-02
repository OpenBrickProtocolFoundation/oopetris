#include "keyboard_input.hpp"

void KeyboardInput::handle_event(const SDL_Event& event, const Window*) {
    m_event_buffer.push_back(event);
}

void KeyboardInput::update(SimulationStep simulation_step_index) {
    for (const auto& event : m_event_buffer) {
        const auto input_event = sdl_event_to_input_event(event);
        if (input_event.has_value()) {
            Input::handle_event(*input_event, simulation_step_index);
        }
    }
    m_event_buffer.clear();

    Input::update(simulation_step_index);
}

helper::optional<InputEvent>
KeyboardInput::sdl_event_to_input_event( // NOLINT(readability-function-cognitive-complexity)
        const SDL_Event& event
) const {
    if (event.type == SDL_KEYDOWN and event.key.repeat == 0) {
        const auto key = event.key.keysym.sym;
        if (key == to_sdl_keycode(m_controls.rotate_left)) {
            return InputEvent::RotateLeftPressed;
        }
        if (key == to_sdl_keycode(m_controls.rotate_right)) {
            return InputEvent::RotateRightPressed;
        }
        if (key == to_sdl_keycode(m_controls.move_down)) {
            return InputEvent::MoveDownPressed;
        }
        if (key == to_sdl_keycode(m_controls.move_left)) {
            return InputEvent::MoveLeftPressed;
        }
        if (key == to_sdl_keycode(m_controls.move_right)) {
            return InputEvent::MoveRightPressed;
        }
        if (key == to_sdl_keycode(m_controls.drop)) {
            return InputEvent::DropPressed;
        }
        if (key == to_sdl_keycode(m_controls.hold)) {
            return InputEvent::HoldPressed;
        }
    } else if (event.type == SDL_KEYUP) {
        const auto key = event.key.keysym.sym;
        if (key == to_sdl_keycode(m_controls.rotate_left)) {
            return InputEvent::RotateLeftReleased;
        }
        if (key == to_sdl_keycode(m_controls.rotate_right)) {
            return InputEvent::RotateRightReleased;
        }
        if (key == to_sdl_keycode(m_controls.move_down)) {
            return InputEvent::MoveDownReleased;
        }
        if (key == to_sdl_keycode(m_controls.move_left)) {
            return InputEvent::MoveLeftReleased;
        }
        if (key == to_sdl_keycode(m_controls.move_right)) {
            return InputEvent::MoveRightReleased;
        }
        if (key == to_sdl_keycode(m_controls.drop)) {
            return InputEvent::DropReleased;
        }
        if (key == to_sdl_keycode(m_controls.hold)) {
            return InputEvent::HoldReleased;
        }
    }
    return helper::nullopt;
}
KeyboardInput::KeyboardInput(KeyboardControls controls, EventDispatcher* event_dispatcher)
    : Input{ InputType::Keyboard },
      m_controls{ controls },
      m_event_dispatcher{ event_dispatcher } {
    m_event_dispatcher->register_listener(this);
}

KeyboardInput::~KeyboardInput() {
    m_event_dispatcher->unregister_listener(this);
}
