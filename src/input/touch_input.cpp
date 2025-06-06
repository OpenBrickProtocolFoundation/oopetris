#include <core/helper/point.hpp>
#include <core/helper/utils.hpp>

#include "input/game_input.hpp"
#include "input/input.hpp"
#include "touch_input.hpp"

#include "helper/spdlog_wrapper.hpp"
#include <cmath>
#include <memory>
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

std::optional<InputEvent> input::TouchGameInput::sdl_event_to_input_event(const SDL_Event& event) {


    if (event.tfinger.touchId != m_underlying_input->m_id) {
        return std::nullopt;
    }

    //TODO(Totto): to handle those things better, holding has to be supported


    if (event.type == SDL_FINGERDOWN) {

        // also take into accounts fingerId, since there may be multiple fingers, each finger has it's own saved state
        const SDL_FingerID finger_id = event.tfinger.fingerId;


        if (m_finger_state.contains(finger_id) and m_finger_state.at(finger_id).has_value()) {
            // there are some valid reasons, this can occur now
            return std::nullopt;
        }

        const auto x_pos = event.tfinger.x;
        const auto y_pos = event.tfinger.y;
        const auto timestamp = event.tfinger.timestamp;

        m_finger_state.insert_or_assign(
                finger_id,
                std::optional<PressedState>{
                        PressedState{ timestamp, x_pos, y_pos }
        }
        );
    }


    if (event.type == SDL_FINGERUP) {

        // also take into accounts fingerId, since there may be multiple fingers, each finger has it's own saved state
        const SDL_FingerID finger_id = event.tfinger.fingerId;


        if (!m_finger_state.contains(finger_id)) {
            // there are some valid reasons, this can occur now
            return std::nullopt;
        }

        const auto& finger_state = m_finger_state.at(finger_id);

        if (!finger_state.has_value()) {
            return std::nullopt;
        }

        const auto& pressed_state = finger_state.value();


        const auto x_pos = event.tfinger.x;
        const auto y_pos = event.tfinger.y;
        const auto timestamp = event.tfinger.timestamp;


        const auto delta_x = x_pos - pressed_state.x;
        const auto delta_y = y_pos - pressed_state.y;
        const auto duration = timestamp - pressed_state.timestamp;

        const auto dx_abs = std::fabs(delta_x);
        const auto dy_abs = std::fabs(delta_y);

        const auto threshold_x = m_settings.move_x_threshold;
        const auto threshold_y = m_settings.move_y_threshold;

        m_finger_state.insert_or_assign(finger_id, std::nullopt);
        if (duration < m_settings.rotation_duration_threshold) {
            if (dx_abs < threshold_x and dy_abs < threshold_y) {
                // tap on the right side of the screen
                if (x_pos > 0.5) {
                    return InputEvent::RotateRightPressed;
                }
                // tap on the left side of the screen
                if (x_pos <= 0.5) {
                    return InputEvent::RotateLeftPressed;
                }
            }
        }

        // swipe right
        if (delta_x > threshold_x and dy_abs < threshold_y) {
            return InputEvent::MoveRightPressed;
        }
        // swipe left
        if (delta_x < -threshold_x and dy_abs < threshold_y) {
            return InputEvent::MoveLeftPressed;
        }
        // swipe down
        if (delta_y > threshold_y and dx_abs < threshold_x) {
            // swipe down to drop
            if (duration < m_settings.drop_duration_threshold) {
                return InputEvent::DropPressed;
            }
            return InputEvent::MoveDownPressed;
        }

        // swipe up
        if (delta_y < -threshold_y and dx_abs < threshold_x) {
            return InputEvent::HoldPressed;
        }
    }


    if (event.type == SDL_FINGERMOTION) {
        //TODO(Totto): support hold
    }


    return std::nullopt;
}


input::TouchGameInput::TouchGameInput(
        const TouchSettings& settings,
        EventDispatcher* event_dispatcher,
        TouchInput* underlying_input
)
    : GameInput{ GameInputType::Touch },
      m_settings{ settings },
      m_event_dispatcher{ event_dispatcher },
      m_underlying_input{ underlying_input } {
    m_event_dispatcher->register_listener(this);
}

input::TouchGameInput::~TouchGameInput() {
    m_event_dispatcher->unregister_listener(this);
}


input::TouchGameInput::TouchGameInput(TouchGameInput&& input) noexcept = default;
[[nodiscard]] input::TouchGameInput& input::TouchGameInput::operator=(TouchGameInput&& input) noexcept = default;

[[nodiscard]] std::optional<input::MenuEvent> input::TouchGameInput::get_menu_event(const SDL_Event& event) const {

    if (event.type == SDL_KEYDOWN and event.key.keysym.sym == SDLK_AC_BACK) {
        return MenuEvent::Pause;
    }

    return std::nullopt;
}

[[nodiscard]] std::string input::TouchGameInput::describe_menu_event(MenuEvent event) const {
    switch (event) {
        case input::MenuEvent::Pause:
            return "Back";
        case input::MenuEvent::OpenSettings:
            throw std::runtime_error("Open Settings is not supported");
        default:
            UNREACHABLE();
    }
}

[[nodiscard]] const input::TouchInput* input::TouchGameInput::underlying_input() const {
    return m_underlying_input;
}

input::TouchInput::TouchInput(const std::shared_ptr<Window>& window, SDL_TouchID touch_id, const std::string& name)
    : PointerInput{ name },
      m_window{ window },
      m_id{ touch_id } { }

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


[[nodiscard]] std::optional<input::NavigationEvent> input::TouchInput::get_navigation_event(const SDL_Event& event
) const {
    //technically no touch event, but it's a navigation event, and by APi design it can also handle those
    if (event.type == SDL_KEYDOWN and event.key.keysym.sym == SDLK_AC_BACK) {
        return input::NavigationEvent::BACK;
    }

    return std::nullopt;
}

[[nodiscard]] std::string input::TouchInput::describe_navigation_event(NavigationEvent event) const {
    switch (event) {
        case NavigationEvent::BACK:
            return "Back";
        case NavigationEvent::OK:
        case NavigationEvent::DOWN:
        case NavigationEvent::UP:
        case NavigationEvent::LEFT:
        case NavigationEvent::RIGHT:
        case NavigationEvent::TAB:
            return "Unsupported";
        default:
            UNREACHABLE();
    }
}

[[nodiscard]] std::optional<input::PointerEventHelper> input::TouchInput::get_pointer_event(const SDL_Event& event
) const {

    auto pointer_event = input::PointerEvent::PointerUp;

    switch (event.type) {
        case SDL_FINGERMOTION:
            pointer_event = input::PointerEvent::Motion;
            break;
        case SDL_FINGERDOWN:
            pointer_event = input::PointerEvent::PointerDown;
            break;
        case SDL_FINGERUP:
            break;
        default:
            return std::nullopt;
    }

    if (event.tfinger.touchId != m_id) {
        return std::nullopt;
    }

    // These are doubles, from 0-1 (or if using virtual layouts > 0) in percent, the have to be casted to absolut x coordinates!
    const double x_percent = event.tfinger.x;
    const double y_percent = event.tfinger.y;
    const auto window_size = m_window->size();
    const auto x_pos = static_cast<i32>(std::round(x_percent * window_size.x));
    const auto y_pos = static_cast<i32>(std::round(y_percent * window_size.y));


    return input::PointerEventHelper{
        shapes::IPoint{ x_pos, y_pos },
        pointer_event
    };
}


[[nodiscard]] SDL_Event input::TouchInput::offset_pointer_event(const SDL_Event& event, const shapes::IPoint& point)
        const {


    auto new_event = event;

    if (event.type != SDL_FINGERMOTION and event.type != SDL_FINGERDOWN and event.type != SDL_FINGERUP) {
        throw std::runtime_error("Tried to offset event, that is no pointer event: in Touch Input");
    }

    using FloatType = decltype(event.tfinger.x);

    const FloatType x_percent = event.tfinger.x;
    const FloatType y_percent = event.tfinger.y;

    const auto window_size = m_window->size();

    new_event.tfinger.x = x_percent + static_cast<FloatType>(point.x) / static_cast<FloatType>(window_size.x);
    new_event.tfinger.y = y_percent + static_cast<FloatType>(point.y) / static_cast<FloatType>(window_size.y);

    return new_event;
}

[[nodiscard]] helper::expected<void, std::string> input::TouchSettings::validate() const {

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

    return {};
}

input::TouchSettings input::TouchSettings::default_settings() {
    return TouchSettings{ .move_x_threshold = 150.0 / 2160.0,
                          .move_y_threshold = 400.0 / 1080.0,
                          .rotation_duration_threshold = 500,
                          .drop_duration_threshold = 200 };
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

    spdlog::debug("Found {} Touch Devices", num_of_touch_devices);

    for (auto i = 0; i < num_of_touch_devices; ++i) {

        auto touch_input = TouchInput::get_by_device_index(window, i);
        if (touch_input.has_value()) {
            inputs.push_back(std::move(touch_input.value()));
        } else {
            spdlog::warn("Failed to get TouchInput: {}", touch_input.error());
        }
    }
}
