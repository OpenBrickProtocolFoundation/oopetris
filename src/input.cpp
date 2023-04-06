#include "input.hpp"
#include "application.hpp"
#include "game_manager.hpp"
#include "key_codes.hpp"

void KeyboardInput::update() {
    Input::update();

    const auto elapsed_time = Application::elapsed_time();
    for (auto& [key, time_point] : m_keys_hold) {
        if (elapsed_time >= time_point) {
            time_point += auto_repeat_rate;
            if (not m_target_game_manager->handle_input_event(static_cast<Event>(key))) {
                time_point = elapsed_time + auto_shift_delay;
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
    if (event.key.keysym.sym == to_sdl_keycode(m_controls.rotate_left)) {
        m_target_game_manager->handle_input_event(Event::RotateLeft);
    } else if (event.key.keysym.sym == to_sdl_keycode(m_controls.rotate_right)) {
        m_target_game_manager->handle_input_event(Event::RotateRight);
    } else if (event.key.keysym.sym == to_sdl_keycode(m_controls.move_down)) {
        m_target_game_manager->handle_input_event(Event::MoveDown);
    } else if (event.key.keysym.sym == to_sdl_keycode(m_controls.move_left)) {
        m_keys_hold[HoldableKey::Left] = Application::elapsed_time() + auto_shift_delay;
        if (not m_target_game_manager->handle_input_event(Event::MoveLeft)) {
            m_keys_hold.erase(HoldableKey::Left);
        }
    } else if (event.key.keysym.sym == to_sdl_keycode(m_controls.move_right)) {
        m_keys_hold[HoldableKey::Right] = Application::elapsed_time() + auto_shift_delay;
        if (not m_target_game_manager->handle_input_event(Event::MoveRight)) {
            m_keys_hold.erase(HoldableKey::Right);
        }
    } else if (event.key.keysym.sym == to_sdl_keycode(m_controls.drop)) {
        m_target_game_manager->handle_input_event(Event::Drop);
    }
}

void KeyboardInput::handle_keyup(const SDL_Event& event) {
    if (event.key.keysym.sym == to_sdl_keycode(m_controls.move_down)) {
        m_target_game_manager->handle_input_event(Event::ReleaseMoveDown);
    } else if (event.key.keysym.sym == to_sdl_keycode(m_controls.move_left)) {
        m_keys_hold.erase(HoldableKey::Left);
    } else if (event.key.keysym.sym == to_sdl_keycode(m_controls.move_right)) {
        m_keys_hold.erase(HoldableKey::Right);
    }
}
