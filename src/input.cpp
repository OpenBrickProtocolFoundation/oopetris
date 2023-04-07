#include "input.hpp"
#include "application.hpp"
#include "game_manager.hpp"
#include "key_codes.hpp"

void KeyboardInput::update() {
    Input::update();

    const auto current_simulation_step_index = Application::simulation_step_index();

    const auto is_left_key_down = m_keys_hold.contains(HoldableKey::Left);
    const auto is_right_key_down = m_keys_hold.contains(HoldableKey::Right);
    if (is_left_key_down and is_right_key_down) {
        return;
    }

    for (auto& [key, target_simulation_step_index] : m_keys_hold) {
        if (current_simulation_step_index >= target_simulation_step_index) {
            while (target_simulation_step_index <= current_simulation_step_index) {
                target_simulation_step_index += auto_repeat_rate_frames;
            }
            if (not m_target_game_manager->handle_input_event(static_cast<Event>(key))) {
                target_simulation_step_index = current_simulation_step_index + delayed_auto_shift_frames;
            }
        }
    }
}

void KeyboardInput::handle_event(const SDL_Event& event) {
    if (event.type == SDL_KEYDOWN and event.key.repeat == 0) {
        handle_keydown(event);
    } else if (event.type == SDL_KEYUP) {
        handle_keyup(event);
    }
}

void KeyboardInput::handle_keydown(const SDL_Event& event) {
    const auto sdl_key = event.key.keysym.sym;
    if (sdl_key == to_sdl_keycode(m_controls.rotate_left)) {
        m_target_game_manager->handle_input_event(Event::RotateLeft);
    } else if (sdl_key == to_sdl_keycode(m_controls.rotate_right)) {
        m_target_game_manager->handle_input_event(Event::RotateRight);
    } else if (sdl_key == to_sdl_keycode(m_controls.move_down)) {
        m_target_game_manager->handle_input_event(Event::MoveDown);
    } else if (sdl_key == to_sdl_keycode(m_controls.move_left)) {
        m_keys_hold[HoldableKey::Left] = Application::simulation_step_index() + delayed_auto_shift_frames;
        if (not m_keys_hold.contains(HoldableKey::Right)
            and not m_target_game_manager->handle_input_event(Event::MoveLeft)) {
            m_keys_hold[HoldableKey::Left] = Application::simulation_step_index();
        }
    } else if (sdl_key == to_sdl_keycode(m_controls.move_right)) {
        m_keys_hold[HoldableKey::Right] = Application::simulation_step_index() + delayed_auto_shift_frames;
        if (not m_keys_hold.contains(HoldableKey::Left)
            and not m_target_game_manager->handle_input_event(Event::MoveRight)) {
            m_keys_hold[HoldableKey::Right] = Application::simulation_step_index();
        }
    } else if (sdl_key == to_sdl_keycode(m_controls.drop)) {
        m_target_game_manager->handle_input_event(Event::Drop);
    }
}

void KeyboardInput::handle_keyup(const SDL_Event& event) {
    const auto sdl_key = event.key.keysym.sym;
    if (sdl_key == to_sdl_keycode(m_controls.move_down)) {
        m_target_game_manager->handle_input_event(Event::ReleaseMoveDown);
    } else if (sdl_key == to_sdl_keycode(m_controls.move_left)) {
        m_keys_hold.erase(HoldableKey::Left);
    } else if (sdl_key == to_sdl_keycode(m_controls.move_right)) {
        m_keys_hold.erase(HoldableKey::Right);
    }
}
