#include "input.hpp"
#include "application.hpp"
#include "game_manager.hpp"
#include <array>

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
        switch (event.key.keysym.sym) {
            case SDLK_LEFT:
                m_target_game_manager->handle_input_event(Event::RotateLeft);
                break;
            case SDLK_RIGHT:
                m_target_game_manager->handle_input_event(Event::RotateRight);
                break;
            case SDLK_s:
                m_target_game_manager->handle_input_event(Event::MoveDown);
                break;
            case SDLK_a:
                m_keys_hold[HoldableKey::Left] = Application::elapsed_time() + auto_shift_delay;
                if (not m_target_game_manager->handle_input_event(Event::MoveLeft)) {
                    m_keys_hold.erase(HoldableKey::Left);
                }
                break;
            case SDLK_d:
                m_keys_hold[HoldableKey::Right] = Application::elapsed_time() + auto_shift_delay;
                if (not m_target_game_manager->handle_input_event(Event::MoveRight)) {
                    m_keys_hold.erase(HoldableKey::Right);
                }
                break;
            case SDLK_SPACE:
            case SDLK_w:
                m_target_game_manager->handle_input_event(Event::Drop);
                break;
        }
    } else if (event.type == SDL_KEYUP) {
        switch (event.key.keysym.sym) {
            case SDLK_s:
                m_target_game_manager->handle_input_event(Event::ReleaseMoveDown);
                break;
            case SDLK_a:
                m_keys_hold.erase(HoldableKey::Left);
                break;
            case SDLK_d:
                m_keys_hold.erase(HoldableKey::Right);
                break;
        }
    }
}
