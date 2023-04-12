#include "input.hpp"
#include "application.hpp"
#include "key_codes.hpp"
#include "recording.hpp"
#include "tetrion.hpp"

void Input::handle_event(const InputEvent event) {
    if (m_on_event_callback) {
        m_on_event_callback(event);
    }

    switch (event) {
        case InputEvent::RotateLeftPressed:
            m_target_tetrion->handle_input_command(InputCommand::RotateLeft);
            break;
        case InputEvent::RotateRightPressed:
            m_target_tetrion->handle_input_command(InputCommand::RotateRight);
            break;
        case InputEvent::MoveLeftPressed:
            m_keys_hold[HoldableKey::Left] = Application::simulation_step_index() + delayed_auto_shift_frames;
            if (not m_keys_hold.contains(HoldableKey::Right)
                and not m_target_tetrion->handle_input_command(InputCommand::MoveLeft)) {
                m_keys_hold[HoldableKey::Left] = Application::simulation_step_index();
            }
            break;
        case InputEvent::MoveRightPressed:
            m_keys_hold[HoldableKey::Right] = Application::simulation_step_index() + delayed_auto_shift_frames;
            if (not m_keys_hold.contains(HoldableKey::Left)
                and not m_target_tetrion->handle_input_command(InputCommand::MoveRight)) {
                m_keys_hold[HoldableKey::Right] = Application::simulation_step_index();
            }
            break;
        case InputEvent::MoveDownPressed:
            m_target_tetrion->handle_input_command(InputCommand::MoveDown);
            break;
        case InputEvent::DropPressed:
            m_target_tetrion->handle_input_command(InputCommand::Drop);
            break;
        case InputEvent::HoldPressed:
            m_target_tetrion->handle_input_command(InputCommand::Hold);
            break;
        case InputEvent::MoveLeftReleased:
            m_keys_hold.erase(HoldableKey::Left);
            break;
        case InputEvent::MoveRightReleased:
            m_keys_hold.erase(HoldableKey::Right);
            break;
        case InputEvent::MoveDownReleased:
            m_target_tetrion->handle_input_command(InputCommand::ReleaseMoveDown);
            break;
        case InputEvent::RotateLeftReleased:
        case InputEvent::RotateRightReleased:
        case InputEvent::DropReleased:
        case InputEvent::HoldReleased:
            break;
        default:
            assert(false and "unreachable");
            break;
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
            if ((key == HoldableKey::Left and not m_target_tetrion->handle_input_command(InputCommand::MoveLeft))
                or (key == HoldableKey::Right and not m_target_tetrion->handle_input_command(InputCommand::MoveRight)
                )) {
                target_simulation_step_index = current_simulation_step_index + delayed_auto_shift_frames;
            }
        }
    }
}

void KeyboardInput::handle_event(const SDL_Event& event) {
    const auto input_event = sdl_event_to_input_event(event);
    if (input_event.has_value()) {
        Input::handle_event(*input_event);
    }
}

tl::optional<InputEvent> KeyboardInput::sdl_event_to_input_event(const SDL_Event& event) const {
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
    return tl::nullopt;
}

ReplayInput::ReplayInput(
        Tetrion* target_tetrion,
        u8 tetrion_index,
        OnEventCallback on_event_callback,
        RecordingReader* recording_reader
)
    : Input{ target_tetrion, std::move(on_event_callback) },
      m_tetrion_index{ tetrion_index },
      m_recording_reader{ recording_reader } { }

void ReplayInput::update() {
    while (true) {
        if (is_end_of_recording()) {
            break;
        }

        const auto& record = m_recording_reader->at(m_next_record_index);

        if (record.tetrion_index != m_tetrion_index) {
            // the current record is not for this tetrion => discard record and keep reading
            ++m_next_record_index;
            continue;
        }

        const auto is_record_for_current_step = (record.simulation_step_index == Application::simulation_step_index());

        if (not is_record_for_current_step) {
            break;
        }

        Input::handle_event(record.event);

        ++m_next_record_index;
    }

    Input::update();
}

void ReplayInput::late_update() {
    Input::late_update();

    while (true) {
        if (m_next_snapshot_index >= m_recording_reader->m_snapshots.size()) {
            break;
        }

        const auto& snapshot = m_recording_reader->m_snapshots.at(m_next_snapshot_index);
        if (snapshot.tetrion_index() != m_tetrion_index) {
            ++m_next_snapshot_index;
            continue;
        }

        // the snapshot corresponds to this tetrion
        assert(snapshot.tetrion_index() == m_tetrion_index);

        if (snapshot.simulation_step_index() != Application::simulation_step_index()) {
            break;
        }

        // create a snapshot from the current state of the tetrion and compare it to the loaded snapshot
        const auto current_snapshot = TetrionSnapshot{ *m_target_tetrion };
#ifdef DEBUG_BUILD
        static constexpr auto verbose_logging = true;
#else
        static constexpr auto verbose_logging = false;
#endif
        if constexpr (verbose_logging) {
            spdlog::info("comparing tetrion snapshots");
        }
        const auto snapshots_are_equal = current_snapshot.compare_to(snapshot, verbose_logging);
        if (snapshots_are_equal) {
            if constexpr (verbose_logging) {
                spdlog::info("snapshots are equal");
            }
        } else {
            spdlog::error("snapshots are not equal");
            throw std::exception{};
        }
        ++m_next_snapshot_index;
    }
}

[[nodiscard]] bool ReplayInput::is_end_of_recording() const {
    return m_next_record_index >= m_recording_reader->num_records();
}
