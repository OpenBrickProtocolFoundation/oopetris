


#include "touch_input.hpp"

#include <cmath>
#include <stdexcept>

void input::TouchGameInput::handle_event(const SDL_Event& event) {
    m_event_buffer.push_back(event);
}

void input::TouchGameInput::update(SimulationStep simulation_step_index) {
    for (const auto& event : m_event_buffer) {
        const auto input_event = sdl_event_to_input_event(event);
        if (input_event.has_value()) {
            GameInput::handle_event(*input_event, simulation_step_index);
        }
    }
    m_event_buffer.clear();

    GameInput::update(simulation_step_index);
}

helper::optional<InputEvent>
input::TouchGameInput::sdl_event_to_input_event( // NOLINT(readability-function-cognitive-complexity)
        const SDL_Event& event
) {
    //TODO to handle those things better, holding has to be supported

    // also take into accounts fingerId, since there may be multiple fingers, each finger has it's own saved state
    const SDL_FingerID finger_id = event.tfinger.fingerId;


    if (event.type == SDL_FINGERDOWN) {
        if (m_finger_state.contains(finger_id) and m_finger_state.at(finger_id).has_value()) {
            // there are some valid reasons, this can occur now
            return helper::nullopt;
        }

        const auto x = event.tfinger.x;
        const auto y = event.tfinger.y;
        const auto timestamp = event.tfinger.timestamp;

        m_finger_state.insert_or_assign(
                finger_id,
                helper::optional<PressedState>{
                        PressedState{ timestamp, x, y }
        }
        );
    }


    if (event.type == SDL_FINGERUP) {
        if (!m_finger_state.contains(finger_id) or !m_finger_state.at(finger_id).has_value()) {
            // there are some valid reasons, this can occur now
            return helper::nullopt;
        }

        const auto pressed_state = m_finger_state.at(finger_id).value();

        const auto x = event.tfinger.x;
        const auto y = event.tfinger.y;
        const auto timestamp = event.tfinger.timestamp;


        const auto dx = x - pressed_state.x;
        const auto dy = y - pressed_state.y;
        const auto duration = timestamp - pressed_state.timestamp;

        const auto dx_abs = std::fabs(dx);
        const auto dy_abs = std::fabs(dy);

        const auto threshold_x = m_settings.move_x_threshold;
        const auto threshold_y = m_settings.move_y_threshold;

        m_finger_state.insert_or_assign(finger_id, helper::nullopt);
        if (duration < m_settings.rotation_duration_threshold) {
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
            if (duration < m_settings.drop_duration_threshold) {
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


    return helper::nullopt;
}


[[nodiscard]] helper::expected<bool, std::string> input::TouchSettings::validate() const {

    if (move_x_threshold > 1.0 || move_x_threshold < 0.0) {
        return helper::unexpected<std::string>{
            fmt::format("move_x_threshold has to be in range [0,1] but was {}", move_x_threshold)
        };
    }

    if (move_y_threshold > 1.0 || move_y_threshold < 0.0) {
        return helper::unexpected<std::string>{
            fmt::format("move_y_threshold has to be in range [0,1] but was {}", move_y_threshold)
        };
    }

    return true;
}


//TODO:
/* 
[[nodiscard]] bool utils::event_is_action(const SDL_Event& event, const CrossPlatformAction action) {
    switch (action) {
        case CrossPlatformAction::OK:
        case CrossPlatformAction::DOWN:
        case CrossPlatformAction::UP:
        case CrossPlatformAction::LEFT:
        case CrossPlatformAction::RIGHT:
        case CrossPlatformAction::OPEN_SETTINGS:
        case CrossPlatformAction::TAB:
            // this can't be checked here, it has to be checked via collision on buttons etc. event_is_action(..., ...::DOWN, UP ...) can only be used inside device_supports_keys() clauses!
            throw std::runtime_error("Not supported on android 'event_is_action'");
        case CrossPlatformAction::PAUSE:
            return (event.type == SDL_KEYDOWN and event.key.keysym.sym == SDLK_AC_BACK);

        case CrossPlatformAction::UNPAUSE:
            return event.type == SDL_FINGERDOWN;

        case CrossPlatformAction::EXIT:
        case CrossPlatformAction::CLOSE:
            return (event.type == SDL_KEYDOWN and event.key.keysym.sym == SDLK_AC_BACK);

        default:
            utils::unreachable();
    }


    [[nodiscard]] std::string utils::action_description(CrossPlatformAction action) {


        switch (action) {
            case CrossPlatformAction::OK:
            case CrossPlatformAction::DOWN:
            case CrossPlatformAction::UP:
            case CrossPlatformAction::LEFT:
            case CrossPlatformAction::RIGHT:
            case CrossPlatformAction::OPEN_SETTINGS:
            case CrossPlatformAction::TAB:
                // this can't be checked here, it has to be checked via collision on buttons etc. event_is_action(..., ...::DOWN, UP ...) can only be used inside device_supports_keys() clauses!
                throw std::runtime_error("Not supported on android 'action_description'");
            case CrossPlatformAction::UNPAUSE:
                return "Tap anywhere";
            case CrossPlatformAction::PAUSE:
            case CrossPlatformAction::EXIT:
            case CrossPlatformAction::CLOSE:
                return "Back";

            default:
                utils::unreachable();
        }
    }
 */
