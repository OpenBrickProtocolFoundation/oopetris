#include "input.hpp"
#include "application.hpp"
#include "game_manager.hpp"
#include "key_codes.hpp"

void Input::handle_command(Input::Command command, Input::CommandType type) {
    if (type == CommandType::KeyDown) {
        switch (command) {
            case Command::MoveLeft:
                m_keys_hold[HoldableKey::Left] = Application::simulation_step_index() + delayed_auto_shift_frames;
                if (not m_keys_hold.contains(HoldableKey::Right)
                    and not m_target_game_manager->handle_input_event(InputEvent::MoveLeft)) {
                    m_keys_hold[HoldableKey::Left] = Application::simulation_step_index();
                }
                break;
            case Command::MoveRight:
                m_keys_hold[HoldableKey::Right] = Application::simulation_step_index() + delayed_auto_shift_frames;
                if (not m_keys_hold.contains(HoldableKey::Left)
                    and not m_target_game_manager->handle_input_event(InputEvent::MoveRight)) {
                    m_keys_hold[HoldableKey::Right] = Application::simulation_step_index();
                }
                break;
            case Command::MoveDown:
                m_target_game_manager->handle_input_event(InputEvent::MoveDown);
                break;
            case Command::RotateLeft:
                m_target_game_manager->handle_input_event(InputEvent::RotateLeft);
                break;
            case Command::RotateRight:
                m_target_game_manager->handle_input_event(InputEvent::RotateRight);
                break;
            case Command::Hold:
                m_target_game_manager->handle_input_event(InputEvent::Hold);
                break;
            case Command::Drop:
                m_target_game_manager->handle_input_event(InputEvent::Drop);
                break;
        }
    } else if (type == CommandType::KeyUp) {
        switch (command) {
            case Command::MoveLeft:
                m_keys_hold.erase(HoldableKey::Left);
                break;
            case Command::MoveRight:
                m_keys_hold.erase(HoldableKey::Right);
                break;
            case Command::MoveDown:
                m_target_game_manager->handle_input_event(InputEvent::ReleaseMoveDown);
                break;
            case Command::RotateLeft:
            case Command::RotateRight:
            case Command::Hold:
            case Command::Drop:
                break;
        }
    } else {
        assert(false and "unreachable");
    }
}

void Input::update() {
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
    const auto command_type = [&]() -> tl::optional<CommandType> {
        if (event.type == SDL_KEYDOWN and event.key.repeat == 0) {
            return CommandType::KeyDown;
        } else if (event.type == SDL_KEYUP) {
            return CommandType::KeyUp;
        }
        return {};
    }();
    const auto is_keydown_or_keyup = command_type.has_value();
    if (is_keydown_or_keyup) {
        const auto sdl_key = event.key.keysym.sym;
        const auto command = sdl_key_to_command(sdl_key);
        const auto is_command = command.has_value();
        if (is_command) {
            handle_command(*command, *command_type);
        }
    }
}

tl::optional<Input::Command> KeyboardInput::sdl_key_to_command(SDL_Keycode key) const {
    if (key == to_sdl_keycode(m_controls.rotate_left)) {
        return Command::RotateLeft;
    } else if (key == to_sdl_keycode(m_controls.rotate_right)) {
        return Command::RotateRight;
    } else if (key == to_sdl_keycode(m_controls.move_down)) {
        return Command::MoveDown;
    } else if (key == to_sdl_keycode(m_controls.move_left)) {
        return Command::MoveLeft;
    } else if (key == to_sdl_keycode(m_controls.move_right)) {
        return Command::MoveRight;
    } else if (key == to_sdl_keycode(m_controls.drop)) {
        return Command::Drop;
    } else if (key == to_sdl_keycode(m_controls.hold)) {
        return Command::Hold;
    }
    return tl::nullopt;
}

ReplayInput::ReplayInput(GameManager* target_game_manager, Recording recording)
    : Input{ target_game_manager },
      m_recording{ std::move(recording) } { }

void ReplayInput::update() {
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
