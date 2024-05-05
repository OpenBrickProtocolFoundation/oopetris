#include "input.hpp"
#include "helper/utils.hpp"
#include "joystick_input.hpp"
#include "keyboard_input.hpp"
#include "manager/settings_manager.hpp"
#include "mouse_input.hpp"
#include "touch_input.hpp"

#include <limits>
#include <memory>
#include <spdlog/spdlog.h>
#include <stdexcept>


[[nodiscard]] bool input::PointerEventHelper::is_in(const shapes::URect& rect) const {
    using Type = decltype(m_pos)::Type;

    assert(rect.top_left.x <= std::numeric_limits<Type>::max());
    assert(rect.top_left.y <= std::numeric_limits<Type>::max());
    assert(rect.bottom_right.x <= std::numeric_limits<Type>::max());
    assert(rect.bottom_right.y <= std::numeric_limits<Type>::max());

    return is_in(rect.cast<Type>());
}

[[nodiscard]] bool input::PointerEventHelper::is_in(const shapes::IRect& rect) const {
    const auto rect_start_x = rect.top_left.x;
    const auto rect_start_y = rect.top_left.y;
    const auto rect_end_x = rect.bottom_right.x;
    const auto rect_end_y = rect.bottom_right.y;


    const bool is_in =
            (m_pos.x >= rect_start_x and m_pos.x <= rect_end_x and m_pos.y >= rect_start_y and m_pos.y <= rect_end_y);

    return is_in;
}


input::InputManager::InputManager(const std::shared_ptr<Window>& window) {

    //initialize mouse input
    m_inputs.push_back(std::make_unique<input::MouseInput>());

    //initialize keyboard input
    m_inputs.push_back(std::make_unique<input::KeyboardInput>());

    //initialize touch input (needs window)
    m_inputs.push_back(std::make_unique<input::TouchInput>(window));

    //initialize joystick input manager
    input::JoyStickInputManager::discover_devices(m_inputs);
}

input::InputManager::~InputManager() = default;


[[nodiscard]] const std::vector<std::unique_ptr<input::Input>>& input::InputManager::inputs() const {
    return m_inputs;
}


[[nodiscard]] helper::optional<input::NavigationEvent> input::InputManager::get_navigation_event(const SDL_Event& event
) const {
    for (const auto& input : m_inputs) {

        if (const auto navigation_event = input->get_navigation_event(event); navigation_event.has_value()) {
            return navigation_event;
        }
    }

    return helper::nullopt;
}

[[nodiscard]] helper::optional<input::PointerEventHelper> input::InputManager::get_pointer_event(const SDL_Event& event
) const {
    for (const auto& input : m_inputs) {
        if (const auto pointer_input = utils::is_child_class<input::PointerInput>(input); pointer_input.has_value()) {
            if (const auto pointer_event = pointer_input.value()->get_pointer_event(event); pointer_event.has_value()) {
                return pointer_event;
            }
        }
    }

    return helper::nullopt;
}


[[nodiscard]] SDL_Event input::InputManager::offset_pointer_event(const SDL_Event& event, const shapes::IPoint& point)
        const {
    for (const auto& input : m_inputs) {
        if (const auto pointer_input = utils::is_child_class<input::PointerInput>(input); pointer_input.has_value()) {
            if (const auto pointer_event = pointer_input.value()->get_pointer_event(event); pointer_event.has_value()) {
                return pointer_input.value()->offset_pointer_event(event, point);
            }
        }
    }

    throw std::runtime_error("Tried to offset event, that is no pointer event");
}


[[nodiscard]] helper::BoolWrapper<input::SpecialRequest> input::InputManager::process_special_inputs(
        const SDL_Event& event
) {
    switch (event.type) {
        case SDL_WINDOWEVENT:
            switch (event.window.event) {
                case SDL_WINDOWEVENT_HIDDEN:
                case SDL_WINDOWEVENT_MINIMIZED:
                case SDL_WINDOWEVENT_LEAVE: {
                    return { true, SpecialRequest::WindowFocusLost };
                }
                default:
                    break;
            }
            break;
        default:
            break;
    }


    const auto is_joystick_special_input = input::JoyStickInputManager::process_special_inputs(event, m_inputs);

    if (is_joystick_special_input) {
        return { true, SpecialRequest::InputsChanged };
    }

    return false;
}

//TODO: improve this API, to correctly use settings to determine the input to use.
[[nodiscard]] std::shared_ptr<input::GameInput> input::InputManager::get_game_input(ServiceProvider* service_provider) {


    //TODO: select the first suitable
    for (const auto& control : service_provider->settings_manager().controls()) {

        return std::visit(
                helper::overloaded{
                        [service_provider](const input::KeyboardSettings& keyboard_settings
                        ) mutable -> std::shared_ptr<GameInput> {
                            auto* const event_dispatcher = &(service_provider->event_dispatcher());
                            auto input =
                                    std::make_shared<input::KeyboardGameInput>(keyboard_settings, event_dispatcher);
                            return input;
                        },
                        [service_provider](const input::JoystickSettings& joystick_settings
                        ) mutable -> std::shared_ptr<GameInput> {
                            auto* const event_dispatcher = &(service_provider->event_dispatcher());
                            auto input = std::make_shared<JoystickGameInput>(joystick_settings, event_dispatcher);
                            return input;
                        },
                        [service_provider](const input::TouchSettings& touch_settings
                        ) mutable -> std::shared_ptr<GameInput> {
                            auto* const event_dispatcher = &(service_provider->event_dispatcher());
                            auto input = std::make_shared<TouchGameInput>(touch_settings, event_dispatcher);
                            return input;
                        } },
                control
        );
    }

    return nullptr;
}
