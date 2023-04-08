#include "input.hpp"
#include "application.hpp"
#include "game_manager.hpp"
#include "key_codes.hpp"
#include "network/network_data.hpp"
#include <array>

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
            if (not m_target_game_manager->handle_input_event(static_cast<InputEvent>(key))) {
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
        m_target_game_manager->handle_input_event(InputEvent::RotateLeft);
    } else if (sdl_key == to_sdl_keycode(m_controls.rotate_right)) {
        m_target_game_manager->handle_input_event(InputEvent::RotateRight);
    } else if (sdl_key == to_sdl_keycode(m_controls.move_down)) {
        m_target_game_manager->handle_input_event(InputEvent::MoveDown);
    } else if (sdl_key == to_sdl_keycode(m_controls.move_left)) {
        m_keys_hold[HoldableKey::Left] = Application::simulation_step_index() + delayed_auto_shift_frames;
        if (not m_keys_hold.contains(HoldableKey::Right)
            and not m_target_game_manager->handle_input_event(InputEvent::MoveLeft)) {
            m_keys_hold[HoldableKey::Left] = Application::simulation_step_index();
        }
    } else if (sdl_key == to_sdl_keycode(m_controls.move_right)) {
        m_keys_hold[HoldableKey::Right] = Application::simulation_step_index() + delayed_auto_shift_frames;
        if (not m_keys_hold.contains(HoldableKey::Left)
            and not m_target_game_manager->handle_input_event(InputEvent::MoveRight)) {
            m_keys_hold[HoldableKey::Right] = Application::simulation_step_index();
        }
    } else if (sdl_key == to_sdl_keycode(m_controls.drop)) {
        m_target_game_manager->handle_input_event(InputEvent::Drop);
    } else if (sdl_key == to_sdl_keycode(m_controls.hold)) {
        m_target_game_manager->handle_input_event(InputEvent::Hold);
    }
}

void KeyboardInput::handle_keyup(const SDL_Event& event) {
    const auto sdl_key = event.key.keysym.sym;
    if (sdl_key == to_sdl_keycode(m_controls.move_down)) {
        m_target_game_manager->handle_input_event(InputEvent::ReleaseMoveDown);
    } else if (sdl_key == to_sdl_keycode(m_controls.move_left)) {
        m_keys_hold.erase(HoldableKey::Left);
    } else if (sdl_key == to_sdl_keycode(m_controls.move_right)) {
        m_keys_hold.erase(HoldableKey::Right);
    }
}

ReplayInput::ReplayInput(GameManager* target_game_manager, Recording recording)
    : Input{ target_game_manager },
      m_recording{ std::move(recording) } { }

void ReplayInput::update() {
    Input::update();

    while (true) {
        if (is_end_of_recording()) {
            break;
        }

        const auto& record = m_recording.at(m_next_record_index);
        const auto is_record_for_current_step = (record.simulation_step_index == Application::simulation_step_index());

        if (not is_record_for_current_step) {
            break;
        }

        m_target_game_manager->handle_input_event(record.event);

        ++m_next_record_index;
    }
}


void OnlineInput::update() {
    Input::update();

    auto data = m_connection->get_data();
    if (!data.has_value()) {
        // TODO: print error here (to log e.g.)
        // auto error = data.error();
        return;
    }

    if (!data.value().has_value()) {
        // no data given
        return;
    }

    const auto data_vector = data.value().value();
    for (const auto& received_data : data_vector) {

        if (received_data.is_of_type<EventData>()) {
            auto event = received_data.as_type<EventData>();
            //TODO maybe handle return value ?
            m_target_game_manager->handle_input_event(event->event());
        }
    }
}