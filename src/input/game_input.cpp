#include "game/tetrion.hpp"
#include "input.hpp"
#include "manager/event_dispatcher.hpp"

#include "helper/spdlog_wrapper.hpp"


void input::GameInput::handle_event(const InputEvent event, const SimulationStep simulation_step_index) {
    if (m_on_event_callback) {
        m_on_event_callback(event, simulation_step_index);
    }

    switch (event) {
        case InputEvent::RotateLeftPressed:
            m_target_tetrion->handle_input_command(GameInputCommand::RotateLeft, simulation_step_index);
            break;
        case InputEvent::RotateRightPressed:
            m_target_tetrion->handle_input_command(GameInputCommand::RotateRight, simulation_step_index);
            break;
        case InputEvent::MoveLeftPressed:
            if (not supports_das()) {
                m_target_tetrion->handle_input_command(GameInputCommand::MoveLeft, simulation_step_index);
            } else {
                m_keys_hold[HoldableKey::Left] = simulation_step_index + delayed_auto_shift_frames;
                if (not m_keys_hold.contains(HoldableKey::Right)
                    and not m_target_tetrion->handle_input_command(GameInputCommand::MoveLeft, simulation_step_index)) {
                    m_keys_hold[HoldableKey::Left] = simulation_step_index;
                }
            }
            break;
        case InputEvent::MoveRightPressed:
            if (not supports_das()) {
                m_target_tetrion->handle_input_command(GameInputCommand::MoveRight, simulation_step_index);
            } else {
                m_keys_hold[HoldableKey::Right] = simulation_step_index + delayed_auto_shift_frames;
                if (not m_keys_hold.contains(HoldableKey::Left)
                    and not m_target_tetrion->handle_input_command(
                            GameInputCommand::MoveRight, simulation_step_index
                    )) {
                    m_keys_hold[HoldableKey::Right] = simulation_step_index;
                }
            }
            break;
        case InputEvent::MoveDownPressed:
            m_target_tetrion->handle_input_command(GameInputCommand::MoveDown, simulation_step_index);
            break;
        case InputEvent::DropPressed:
            m_target_tetrion->handle_input_command(GameInputCommand::Drop, simulation_step_index);
            break;
        case InputEvent::HoldPressed:
            m_target_tetrion->handle_input_command(GameInputCommand::Hold, simulation_step_index);
            break;
        case InputEvent::MoveLeftReleased:
            if (supports_das()) {
                m_keys_hold.erase(HoldableKey::Left);
            }
            break;
        case InputEvent::MoveRightReleased:
            if (supports_das()) {
                m_keys_hold.erase(HoldableKey::Right);
            }
            break;
        case InputEvent::MoveDownReleased:
            m_target_tetrion->handle_input_command(GameInputCommand::ReleaseMoveDown, simulation_step_index);
            break;
        case InputEvent::RotateLeftReleased:
        case InputEvent::RotateRightReleased:
        case InputEvent::DropReleased:
        case InputEvent::HoldReleased:
            break;
        default:
            UNREACHABLE();
    }
}

input::GameInput::GameInput(GameInput&&) = default;

input::GameInput& input::GameInput::operator=(GameInput&&) = default;

input::GameInput::~GameInput() = default;

void input::GameInput::update(const SimulationStep simulation_step_index) {
    const auto current_simulation_step_index = simulation_step_index;

    const auto is_left_key_down = m_keys_hold.contains(HoldableKey::Left);
    const auto is_right_key_down = m_keys_hold.contains(HoldableKey::Right);
    if (is_left_key_down and is_right_key_down) {
        return;
    }

    for (auto& [key, target_simulation_step_index] : m_keys_hold) {
        if (current_simulation_step_index >= target_simulation_step_index) {
            while (target_simulation_step_index <= current_simulation_step_index) { // NOLINT(bugprone-infinite-loop)
                target_simulation_step_index += auto_repeat_rate_frames;
            }
            if ((key == HoldableKey::Left
                 and not m_target_tetrion->handle_input_command(GameInputCommand::MoveLeft, simulation_step_index))
                or (key == HoldableKey::Right
                    and not m_target_tetrion->handle_input_command(GameInputCommand::MoveRight, simulation_step_index)
                )) {
                target_simulation_step_index = current_simulation_step_index + delayed_auto_shift_frames;
            }
        }
    }
}


void input::GameInput::late_update(SimulationStep /*simulation_step*/) {
    //do nothing, is expected here, this is virtual, so if there is soemthing to do, it gets overridden
}
