


#include "touch_input.hpp"

#include <cmath>
#include <memory>
#include <spdlog/spdlog.h>
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


input::TouchInput::TouchInput(const std::shared_ptr<Window>& window, SDL_TouchID id, const std::string& name)
    : PointerInput{ name },
      m_window{ window },
      m_id{ id } { }


[[nodiscard]] helper::expected<std::unique_ptr<input::TouchInput>, std::string>
input::TouchInput::get_by_device_index(const std::shared_ptr<Window>& window, int device_index) {


    const auto touch_id = SDL_GetTouchDevice(device_index);

    if (touch_id <= 0) {
        return helper::unexpected<std::string>{
            fmt::format("Failed to get touch id at device index {}: {}", device_index, SDL_GetError())
        };
    }


    std::string name = "unknown name";
    const auto* char_name = SDL_GetTouchName(device_index);

    if (char_name != nullptr) {
        name = char_name;
    }

    return std::make_unique<TouchInput>(window, touch_id, name);
}


[[nodiscard]] helper::optional<input::NavigationEvent> input::TouchInput::get_navigation_event(const SDL_Event& event) const {

}

[[nodiscard]] std::string input::TouchInput::describe_navigation_event(NavigationEvent event) const { }

[[nodiscard]] helper::optional<input::PointerEventHelper> input::TouchInput::get_pointer_event(const SDL_Event& event) const {

}

[[nodiscard]] SDL_Event input::TouchInput::offset_pointer_event(const SDL_Event& event, const shapes::IPoint& point)
        const {

            
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


[[nodiscard]] SDL_Event input::TouchInput::offset_pointer_event(const SDL_Event& event, const shapes::IPoint& point)
        const {


    auto new_event = event;

    if (event.type != SDL_FINGERMOTION and event.type != SDL_FINGERDOWN and event.type != SDL_FINGERUP) {
        throw std::runtime_error("Tried to offset event, that is no pointer event: in Touch Input");
    }


    const double x_percent = event.tfinger.x;
    const double y_percent = event.tfinger.y;

    const auto window_size = m_window->size();

    new_event.tfinger.x = x_percent + static_cast<double>(point.x) / static_cast<double>(window_size.x);
    new_event.tfinger.y = y_percent + static_cast<double>(point.y) / static_cast<double>(window_size.y);

    return new_event;
}


void input::TouchInputManager::discover_devices(
        std::vector<std::unique_ptr<Input>>& inputs,
        const std::shared_ptr<Window>& window
) {


    const auto num_of_touch_devices = SDL_GetNumTouchDevices();

    if (num_of_touch_devices < 0) {
        spdlog::warn("Failed to get number of touch devices: {}", SDL_GetError());
        return;
    }

    for (auto i = 0; i < num_of_touch_devices; ++i) {

        auto touch_input = TouchInput::get_by_device_index(window, i);
        if (touch_input.has_value()) {
            inputs.push_back(std::move(touch_input.value()));
        } else {
            spdlog::warn("Failed to get TouchInput: {}", touch_input.error());
        }
    }
}


//TODO:
/* 


    decltype(event.type) desired_type{};
    switch (click_type) {
        case CrossPlatformClickEvent::Motion:
            desired_type = SDL_FINGERMOTION;
            break;
        case CrossPlatformClickEvent::ButtonDown:
            desired_type = SDL_FINGERDOWN;
            break;
        case CrossPlatformClickEvent::ButtonUp:
            desired_type = SDL_FINGERUP;
            break;
        case CrossPlatformClickEvent::Any:
            return event.type == SDL_FINGERMOTION || event.type == SDL_FINGERDOWN || event.type == SDL_FINGERUP;
        default:
            utils::unreachable();
    }

    return event.type == desired_type;
 */


/**
 


[[nodiscard]] std::pair<i32, i32> utils::get_raw_coordinates(const Window* window, const SDL_Event& event) {

    assert(utils::event_is_click_event(event, utils::CrossPlatformClickEvent::Any) && "expected a click event");

#if defined(__ANDROID__)
    // These are doubles, from 0-1 (or if using virtual layouts > 0) in percent, the have to be casted to absolut x coordinates!
    const double x_percent = event.tfinger.x;
    const double y_percent = event.tfinger.y;
    const auto window_size = window->size();
    const auto x = static_cast<i32>(std::round(x_percent * window_size.x));
    const auto y = static_cast<i32>(std::round(y_percent * window_size.y));


#elif defined(__SWITCH__)
    UNUSED(window);
    UNUSED(event);
    throw std::runtime_error("Not supported on the Nintendo switch");
    int x{};
    int y{};
#else
    UNUSED(window);

    Sint32 x{};
    Sint32 y{};
    switch (event.type) {
        case SDL_MOUSEMOTION:
            x = event.motion.x;
            y = event.motion.y;
            break;
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
            x = event.button.x;
            y = event.button.y;
            break;
        default:
            utils::unreachable();
    }
#endif


    return { static_cast<i32>(x), static_cast<i32>(y) };
}


 * 
 */


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
