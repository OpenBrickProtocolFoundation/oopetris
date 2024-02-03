#include "input.hpp"
#include "event_dispatcher.hpp"
#include "key_codes.hpp"
#include "recording.hpp"
#include "tetrion.hpp"
#include <spdlog/spdlog.h>

#if defined(__ANDROID__)
#include <cmath>
#include <stdexcept>
#include <tl/optional.hpp>
#endif

#if defined(__SWITCH__)
#include "switch_buttons.hpp"
#endif

void Input::handle_event(const InputEvent event, const SimulationStep simulation_step_index) {
    if (m_on_event_callback) {
        m_on_event_callback(event);
    }

    switch (event) {
        case InputEvent::RotateLeftPressed:
            m_target_tetrion->handle_input_command(InputCommand::RotateLeft, simulation_step_index);
            break;
        case InputEvent::RotateRightPressed:
            m_target_tetrion->handle_input_command(InputCommand::RotateRight, simulation_step_index);
            break;
        case InputEvent::MoveLeftPressed:
#if defined(__ANDROID__)
            m_target_tetrion->handle_input_command(InputCommand::MoveLeft, simulation_step_index);
#else
            m_keys_hold[HoldableKey::Left] = simulation_step_index + delayed_auto_shift_frames;
            if (not m_keys_hold.contains(HoldableKey::Right)
                and not m_target_tetrion->handle_input_command(InputCommand::MoveLeft, simulation_step_index)) {
                m_keys_hold[HoldableKey::Left] = simulation_step_index;
            }
#endif
            break;
        case InputEvent::MoveRightPressed:
#if defined(__ANDROID__)
            m_target_tetrion->handle_input_command(InputCommand::MoveRight, simulation_step_index);
#else
            m_keys_hold[HoldableKey::Right] = simulation_step_index + delayed_auto_shift_frames;
            if (not m_keys_hold.contains(HoldableKey::Left)
                and not m_target_tetrion->handle_input_command(InputCommand::MoveRight, simulation_step_index)) {
                m_keys_hold[HoldableKey::Right] = simulation_step_index;
            }
#endif
            break;
        case InputEvent::MoveDownPressed:
            m_target_tetrion->handle_input_command(InputCommand::MoveDown, simulation_step_index);
            break;
        case InputEvent::DropPressed:
            m_target_tetrion->handle_input_command(InputCommand::Drop, simulation_step_index);
            break;
        case InputEvent::HoldPressed:
            m_target_tetrion->handle_input_command(InputCommand::Hold, simulation_step_index);
            break;
        case InputEvent::MoveLeftReleased:
#if not defined(__ANDROID__)
            m_keys_hold.erase(HoldableKey::Left);
#endif
            break;
        case InputEvent::MoveRightReleased:
#if not defined(__ANDROID__)
            m_keys_hold.erase(HoldableKey::Right);
#endif
            break;
        case InputEvent::MoveDownReleased:
            m_target_tetrion->handle_input_command(InputCommand::ReleaseMoveDown, simulation_step_index);
            break;
        case InputEvent::RotateLeftReleased:
        case InputEvent::RotateRightReleased:
        case InputEvent::DropReleased:
        case InputEvent::HoldReleased:
            break;
        default:
            utils::unreachable();
    }
}

void Input::update(const SimulationStep simulation_step_index) {
    const auto current_simulation_step_index = simulation_step_index;

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
            if ((key == HoldableKey::Left
                 and not m_target_tetrion->handle_input_command(InputCommand::MoveLeft, simulation_step_index))
                or (key == HoldableKey::Right
                    and not m_target_tetrion->handle_input_command(InputCommand::MoveRight, simulation_step_index))) {
                target_simulation_step_index = current_simulation_step_index + delayed_auto_shift_frames;
            }
        }
    }
}

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
KeyboardInput::KeyboardInput(
        Tetrion* target_tetrion,
        KeyboardControls controls,
        EventDispatcher* event_dispatcher,
        Input::OnEventCallback on_event_callback
)
    : Input{ target_tetrion, std::move(on_event_callback) },
      m_controls{ controls },
      m_event_dispatcher{ event_dispatcher } {
    m_event_dispatcher->register_listener(this);
}

KeyboardInput::~KeyboardInput() {
    m_event_dispatcher->unregister_listener(this);
}

ReplayInput::ReplayInput(
        Tetrion* const target_tetrion,
        OnEventCallback on_event_callback,
        RecordingReader* const recording_reader
)
    : Input{ target_tetrion, std::move(on_event_callback) },
      m_recording_reader{ recording_reader } { }

void ReplayInput::update(const SimulationStep simulation_step_index) {
    while (true) {
        if (is_end_of_recording()) {
            break;
        }

        const auto& record = m_recording_reader->at(m_next_record_index);

        if (record.tetrion_index != m_target_tetrion->tetrion_index()) {
            // the current record is not for this tetrion => discard record and keep reading
            ++m_next_record_index;
            continue;
        }

        const auto is_record_for_current_step = (record.simulation_step_index == simulation_step_index);

        if (not is_record_for_current_step) {
            break;
        }

        spdlog::debug("replaying event {} at step {}", magic_enum::enum_name(record.event), simulation_step_index);

        Input::handle_event(record.event, simulation_step_index);

        ++m_next_record_index;
    }

    Input::update(simulation_step_index);
}

void ReplayInput::late_update(const SimulationStep simulation_step_index) {
    Input::late_update(simulation_step_index);

    while (true) {
        if (m_next_snapshot_index >= m_recording_reader->m_snapshots.size()) {
            break;
        }

        const auto& snapshot = m_recording_reader->m_snapshots.at(m_next_snapshot_index);
        if (snapshot.tetrion_index() != m_target_tetrion->tetrion_index()) {
            ++m_next_snapshot_index;
            continue;
        }

        // the snapshot corresponds to this tetrion
        assert(snapshot.tetrion_index() == m_target_tetrion->tetrion_index());

        if (snapshot.simulation_step_index() != simulation_step_index) {
            break;
        }

        // create a snapshot from the current state of the tetrion and compare it to the loaded snapshot
        const auto current_snapshot = TetrionSnapshot{ *m_target_tetrion, simulation_step_index };
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


#if defined(__ANDROID__)

void TouchInput::handle_event(const SDL_Event& event) {
    m_event_buffer.push_back(event);
}

void TouchInput::update(SimulationStep simulation_step_index) {
    for (const auto& event : m_event_buffer) {
        const auto input_event = sdl_event_to_input_event(event);
        if (input_event.has_value()) {
            Input::handle_event(*input_event, simulation_step_index);
        }
    }
    m_event_buffer.clear();

    Input::update(simulation_step_index);
}

tl::optional<InputEvent> TouchInput::sdl_event_to_input_event(const SDL_Event& event) {
    //TODO to handle those things better, holding has to be supported

    // also take into accounts fingerId, since there may be multiple fingers, each finger has it's own saved state
    const SDL_FingerID finger_id = event.tfinger.fingerId;

    // this is used, to get the percentage, since it' all constexpr it's mainly for the developer to not think about percentages but about a pixel range on a certain device, but then it works as expected everywhere
    constexpr auto screen_h_reference = 2160.0;
    constexpr auto screen_w_reference = 1080.0;

    if (event.type == SDL_FINGERDOWN) {
        if (m_finger_state.contains(finger_id) and m_finger_state.at(finger_id).has_value()) {
            // there are some valid reasons, this can occur now
            return tl::nullopt;
        }

        const auto x = event.tfinger.x;
        const auto y = event.tfinger.y;
        const auto timestamp = event.tfinger.timestamp;

        m_finger_state.insert_or_assign(finger_id, tl::make_optional(PressedState{ timestamp, x, y }));
    }


    if (event.type == SDL_FINGERUP) {
        if (!m_finger_state.contains(finger_id) or !m_finger_state.at(finger_id).has_value()) {
            // there are some valid reasons, this can occur now
            return tl::nullopt;
        }

        const auto pressed_state = m_finger_state.at(finger_id).value();

        const auto x = event.tfinger.x;
        const auto y = event.tfinger.y;
        const auto timestamp = event.tfinger.timestamp;

        constexpr auto threshold_x = 150.0 / screen_w_reference;
        constexpr auto threshold_y = 400.0 / screen_h_reference;
        constexpr auto duration_threshold = 500.0;
        constexpr auto duration_drop_threshold = 200.0;

        const auto dx = x - pressed_state.x;
        const auto dy = y - pressed_state.y;
        const auto duration = timestamp - pressed_state.timestamp;

        const auto dx_abs = std::fabs(dx);
        const auto dy_abs = std::fabs(dy);

        m_finger_state.insert_or_assign(finger_id, tl::nullopt);
        if (duration < duration_threshold) {
            if (dx_abs < threshold_x and dy_abs < threshold_y) {
                // tap on the right side of the screen
                if (x > 0.5) {
                    return InputEvent::RotateRightPressed;
                }
                // tap on the left side of the screen
                if (x <= 0.5) {
                    return InputEvent::RotateLeftPressed;
                }
            }
        }

        // swipe right
        if (dx > threshold_x and dy_abs < threshold_y) {
            return InputEvent::MoveRightPressed;
        }
        // swipe left
        if (dx < -threshold_x and dy_abs < threshold_y) {
            return InputEvent::MoveLeftPressed;
        }
        // swipe down
        if (dy > threshold_y and dx_abs < threshold_x) {
            // swipe down to drop
            if (duration < duration_drop_threshold) {
                return InputEvent::DropPressed;
            }
            return InputEvent::MoveDownPressed;
        }

        // swipe up
        if (dy < -threshold_y and dx_abs < threshold_x) {
            return InputEvent::HoldPressed;
        }
    }


    if (event.type == SDL_FINGERMOTION) {
        //TODO support hold
    }


    return tl::nullopt;
}
#endif


#if defined(__SWITCH__)


void JoystickInput::handle_event(const SDL_Event& event) {
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

tl::optional<InputEvent> JoystickInput::sdl_event_to_input_event(const SDL_Event& event) const {
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
    return tl::nullopt;
}


#endif
