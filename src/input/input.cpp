#include "input.hpp"
#include "helper/expected.hpp"
#include "helper/optional.hpp"
#include "helper/utils.hpp"
#include "joystick_input.hpp"
#include "keyboard_input.hpp"
#include "manager/settings_manager.hpp"
#include "mouse_input.hpp"
#include "touch_input.hpp"

#include <cassert>
#include <limits>
#include <memory>
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <string>


input::Input::Input(std::string name, InputType type) : m_name{ std::move(name) }, m_type{ type } { }

input::Input::~Input() = default;

input::PointerEventHelper::PointerEventHelper(shapes::IPoint pos, PointerEvent event)
    : m_pos{ pos },
      m_event{ event } { }

[[nodiscard]] input::PointerEvent input::PointerEventHelper::event() const {
    return m_event;
}

[[nodiscard]] shapes::IPoint input::PointerEventHelper::position() const {
    return m_pos;
}

[[nodiscard]] bool input::PointerEventHelper::is_in(const shapes::URect& rect) const {
    using Type = decltype(m_pos)::Type;

    assert(rect.top_left.x <= static_cast<u32>(std::numeric_limits<Type>::max()));
    assert(rect.top_left.y <= static_cast<u32>(std::numeric_limits<Type>::max()));
    assert(rect.bottom_right.x <= static_cast<u32>(std::numeric_limits<Type>::max()));
    assert(rect.bottom_right.y <= static_cast<u32>(std::numeric_limits<Type>::max()));

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

[[nodiscard]] bool input::PointerEventHelper::operator==(PointerEvent event) const {
    return m_event == event;
}

input::InputManager::InputManager(const std::shared_ptr<Window>& window) {

    //initialize mouse input
    m_inputs.push_back(std::make_unique<input::MouseInput>());

    //initialize keyboard input
    m_inputs.push_back(std::make_unique<input::KeyboardInput>());

    //initialize touch inputs by using the manager(needs window)
    input::TouchInputManager::discover_devices(m_inputs, window);

    //initialize joystick inputs by using the manager
    input::JoyStickInputManager::discover_devices(m_inputs);
}

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

namespace {
#if defined(__ANDROID__)
    using PrimaryInputType = input::TouchInput;
#elif defined(__CONSOLE__)
    using PrimaryInputType = input::JoystickInput;
#else
    using PrimaryInputType = input::KeyboardInput;
#endif
} // namespace


namespace {
    //TODO(Totto): use smart pointer for the event dispatcher
    helper::optional<std::shared_ptr<input::GameInput>>
    get_game_input_by_setting(ServiceProvider* service_provider, const Controls& control) {

        using ReturnType = helper::expected<std::shared_ptr<input::GameInput>, std::string>;


        auto result = std::visit(
                helper::overloaded{
                        [service_provider](const input::KeyboardSettings& keyboard_settings) mutable -> ReturnType {
                            auto* const event_dispatcher = &(service_provider->event_dispatcher());
                            return std::make_shared<input::KeyboardGameInput>(keyboard_settings, event_dispatcher);
                        },
                        [service_provider](const input::JoystickSettings& joystick_settings) mutable -> ReturnType {
                            auto* const event_dispatcher = &(service_provider->event_dispatcher());
                            auto input = input::JoystickGameInput::get_game_input_by_settings(
                                    service_provider->input_manager(), event_dispatcher, joystick_settings
                            );

                            if (not input.has_value()) {
                                return helper::unexpected<std::string>{
                                    fmt::format("Not possible to get joystick by settings: {}", input.error())
                                };
                            }


                            return input.value();
                        },
                        [service_provider](const input::TouchSettings& touch_settings) mutable -> ReturnType {
                            //TODO(Totto): make it dynamic, which touch input to use

                            for (const auto& input : service_provider->input_manager().inputs()) {
                                if (const auto pointer_input = utils::is_child_class<input::TouchInput>(input);
                                    pointer_input.has_value()) {
                                    auto* const event_dispatcher = &(service_provider->event_dispatcher());
                                    return std::make_shared<input::TouchGameInput>(
                                            touch_settings, event_dispatcher, pointer_input.value()
                                    );
                                }
                            }

                            return helper::unexpected<std::string>{
                                "No TouchInput was found, so no TouchGameInput can be created"
                            };
                        } },
                control
        );


        if (result.has_value()) {
            return std::move(result.value());
        }

        return helper::nullopt;
    }


    template<typename T>
    T get_settings_or_default(const std::vector<Controls>& controls) {
        for (const auto& control : controls) {

            const T* retrieved = std::get_if<T>(&control);

            if (retrieved != nullptr) {
                return *retrieved;
            }
        }


        return T::default_settings();
    }


    input::JoystickSettings
    get_settings_or_default_joystick(const input::JoystickInput* input, const std::vector<Controls>& controls) {

        for (const auto& control : controls) {

            const auto* retrieved = std::get_if<input::JoystickSettings>(&control);

            if (retrieved != nullptr) {
                if (retrieved->identification.guid == input->guid()) {
                    return *retrieved;
                }
            }
        }


        return input->default_settings();
    }


    helper::optional<std::shared_ptr<input::GameInput>>
    get_game_input_by_input(ServiceProvider* service_provider, const std::unique_ptr<input::Input>& input) {

        const auto& settings = service_provider->settings_manager().settings();


        if (const auto keyboard_input = utils::is_child_class<input::KeyboardInput>(input);
            keyboard_input.has_value()) {

            const auto keyboard_settings = get_settings_or_default<input::KeyboardSettings>(settings.controls);

            auto* const event_dispatcher = &(service_provider->event_dispatcher());
            return std::make_shared<input::KeyboardGameInput>(keyboard_settings, event_dispatcher);
        }


        if (const auto joystick_input = utils::is_child_class<input::JoystickInput>(input);
            joystick_input.has_value()) {

            const auto joystick_settings = get_settings_or_default_joystick(joystick_input.value(), settings.controls);

            auto* const event_dispatcher = &(service_provider->event_dispatcher());

            auto game_input = input::JoystickGameInput::get_game_input_by_settings(
                    service_provider->input_manager(), event_dispatcher, joystick_settings
            );

            if (not game_input.has_value()) {
                spdlog::warn("Not possible to get joystick by settings: {}", game_input.error());
                return helper::nullopt;
            }


            return game_input.value();
        }

        if (const auto touch_input = utils::is_child_class<input::TouchInput>(input); touch_input.has_value()) {

            const auto touch_settings = get_settings_or_default<input::TouchSettings>(settings.controls);

            auto* const event_dispatcher = &(service_provider->event_dispatcher());

            return std::make_shared<input::TouchGameInput>(touch_settings, event_dispatcher, touch_input.value());
        }


        return helper::nullopt;
    }


} // namespace


[[nodiscard]] helper::optional<std::shared_ptr<input::GameInput>> input::InputManager::get_game_input(
        ServiceProvider* service_provider
) {

    const auto& settings = service_provider->settings_manager().settings();

    // 1. If we have a fixed index, by settings, we use that, if it fails, we don#t try anything other

    if (settings.selected.has_value()) {
        const auto index = settings.selected.value();
        if (settings.controls.size() >= index) {
            return helper::nullopt;
        }

        return get_game_input_by_setting(service_provider, settings.controls.at(index));
    }


    // 2. We use the primary input for this platform

    for (const auto& input : service_provider->input_manager().inputs()) {
        if (auto primary_input = utils::is_child_class<PrimaryInputType>(input); primary_input.has_value()) {
            auto result = get_game_input_by_input(service_provider, input);
            if (result.has_value()) {
                return result.value();
            }
        }
    }


    // 3. we fail, since no suitable input could be found

    return helper::nullopt;
}

[[nodiscard]] const std::unique_ptr<input::Input>& input::InputManager::get_primary_input() {

    for (const auto& input : m_inputs) {
        if (const auto pointer_input = utils::is_child_class<PrimaryInputType>(input); pointer_input.has_value()) {
            return input;
        }
    }

    // this should always be true, since in the initialization the first one, that is ALWAYS added is the KeyboardInput
    assert(not m_inputs.empty() && "at least one input has to be given");
    return m_inputs.at(0);
}

input::PointerInput::PointerInput(const std::string& name) : Input{ name, input::InputType::Pointer } { }
