
#include <core/helper/string_manipulation.hpp>

#include "controller_input.hpp"
#include "input/input.hpp"
#include "input/joystick_input.hpp"
#include "manager/sdl_controller_key.hpp"


#include <spdlog/spdlog.h>

input::ControllerInput::ControllerInput(
        SDL_GameController* controller,
        SDL_JoystickID instance_id,
        const std::string& name
)
    : input::JoystickLikeInput{ instance_id, name, input::JoystickLikeType::Controller },
      m_controller{ controller } { }


input::ControllerInput::~ControllerInput() {
    SDL_GameControllerClose(m_controller);
}


input::ControllerInput::ControllerInput(const ControllerInput& input) noexcept = default;
input::ControllerInput& input::ControllerInput::operator=(const ControllerInput& input) noexcept = default;

input::ControllerInput::ControllerInput(ControllerInput&& input) noexcept = default;
input::ControllerInput& input::ControllerInput::operator=(ControllerInput&& input) noexcept = default;


[[nodiscard]] helper::expected<std::unique_ptr<input::ControllerInput>, std::string>
input::ControllerInput::get_by_device_index(int device_index) {

    const auto is_game_controller = SDL_IsGameController(device_index);

    if (is_game_controller != SDL_TRUE) {
        return helper::unexpected<std::string>{
            fmt::format("This Joystick is not a Game Controller: {}", device_index)
        };
    }

    auto* game_controller = SDL_GameControllerOpen(device_index);

    if (game_controller == nullptr) {
        return helper::unexpected<std::string>{
            fmt::format("Failed to get game_controller at device index {}: {}", device_index, SDL_GetError())
        };
    }


    auto* underlying_joystick = SDL_GameControllerGetJoystick(game_controller);

    if (underlying_joystick == nullptr) {
        return helper::unexpected<std::string>{
            fmt::format("Failed to get underlying joystick from game controller {}: {}", device_index, SDL_GetError())
        };
    }


    const auto instance_id = SDL_JoystickInstanceID(underlying_joystick);

    if (instance_id < 0) {
        return helper::unexpected<std::string>{ fmt::format("Failed to get joystick instance id: {}", SDL_GetError()) };
    }

    std::string name = "unknown name";
    const auto* char_name = SDL_GameControllerName(game_controller);

    if (char_name != nullptr) {
        name = char_name;
    }


    return std::make_unique<ControllerInput>(game_controller, instance_id, name);
}


[[nodiscard]] std::optional<input::NavigationEvent> input::ControllerInput::get_navigation_event(const SDL_Event& event
) const {
    if (event.type == SDL_CONTROLLERBUTTONDOWN) {

        if (event.cbutton.which != instance_id()) {
            return std::nullopt;
        }

        switch (event.cbutton.button) {
            case SDL_CONTROLLER_BUTTON_A:
                return NavigationEvent::OK;
            case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
                return NavigationEvent::DOWN;
            case SDL_CONTROLLER_BUTTON_DPAD_UP:
                return NavigationEvent::UP;
            case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
                return NavigationEvent::LEFT;
            case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
                return NavigationEvent::RIGHT;
            case SDL_CONTROLLER_BUTTON_BACK:
                return NavigationEvent::BACK;
            default:
                return std::nullopt;

                //note, that  NavigationEvent::TAB is not supported
        }
    }

    return handle_axis_navigation_event(event);
}

[[nodiscard]] std::string input::ControllerInput::describe_navigation_event(NavigationEvent event) const {
    switch (event) {
        case NavigationEvent::OK:
            return "A";
        case NavigationEvent::BACK:
            return "Back";
        case NavigationEvent::DOWN:
            return "Down";
        case NavigationEvent::UP:
            return "Up";
        case NavigationEvent::LEFT:
            return "Left";
        case NavigationEvent::RIGHT:
            return "Right";
        case NavigationEvent::TAB:
            throw std::runtime_error("Tab is not supported");
        default:
            UNREACHABLE();
    }
}


[[nodiscard]] std::optional<input::NavigationEvent> input::ControllerInput::handle_axis_navigation_event(
        const SDL_Event& event
) const {
    if (event.type == SDL_CONTROLLERAXISMOTION) {

        //TODO(Totto). maybe make this configurable
        // this constant is here, that slight touches aren't counted as inputs ( really slight wiggles might occur unintentinoally) NOTE: that most inputs use all 16 bits for a normal press, so that this value can be that "big"!
        //note: most implementations for a specific controller emit SDL_JOYSTICK_AXIS_MAX or 0 most of the time
        constexpr double axis_threshold_percentage = 0.7;
        constexpr auto axis_threshold =
                static_cast<i16>(static_cast<double>(SDL_JOYSTICK_AXIS_MAX) * axis_threshold_percentage);

        if (event.caxis.which != instance_id()) {
            return std::nullopt;
        }

        // x axis movement
        if (event.caxis.axis == SDL_CONTROLLER_AXIS_LEFTX or event.caxis.axis == SDL_CONTROLLER_AXIS_RIGHTX) {
            if (event.caxis.value > axis_threshold) {
                return NavigationEvent::RIGHT;
            }

            if (event.caxis.value < -axis_threshold) {
                return NavigationEvent::LEFT;
            }

            return std::nullopt;
        }

        // y axis movement
        if (event.caxis.axis == SDL_CONTROLLER_AXIS_LEFTY or event.caxis.axis == SDL_CONTROLLER_AXIS_RIGHTY) {
            if (event.caxis.value > axis_threshold) {
                return NavigationEvent::DOWN;
            }

            if (event.caxis.value < -axis_threshold) {
                return NavigationEvent::UP;
            }

            return std::nullopt;
        }

        //Note: not all types of SDL_GameControllerAxis are handled, since they are not needed for navigation events

        return std::nullopt;
    }

    return std::nullopt;
}


[[nodiscard]] helper::expected<void, std::string> input::ControllerSettings::validate() const {

    const std::vector<sdl::ControllerKey> to_use{ rotate_left, rotate_right, move_left, move_right,   move_down,
                                                  drop,        hold,         pause,     open_settings };

    return input::InputSettings::has_unique_members(to_use);
}


input::ControllerGameInput::ControllerGameInput(
        ControllerSettings settings,
        EventDispatcher* event_dispatcher,
        ControllerInput* underlying_input
)
    : JoystickLikeGameInput{ event_dispatcher, JoystickLikeType::Controller },
      m_settings{ settings },
      m_underlying_input{ underlying_input } { }

[[nodiscard]] const input::ControllerInput* input::ControllerGameInput::underlying_input() const {
    return m_underlying_input;
}


[[nodiscard]] std::optional<input::MenuEvent> input::ControllerGameInput::get_menu_event(const SDL_Event& event) const {
    if (event.type == SDL_CONTROLLERBUTTONDOWN) {

        if (event.cbutton.which != underlying_input()->instance_id()) {
            return std::nullopt;
        }

        const auto button = sdl::ControllerKey{ static_cast<SDL_GameControllerButton>(event.cbutton.button) };

        if (button == m_settings.pause) {
            return MenuEvent::Pause;
        }
        if (button == m_settings.open_settings) {
            return MenuEvent::OpenSettings;
        }
    }

    return std::nullopt;
    //
}

[[nodiscard]] std::string input::ControllerGameInput::describe_menu_event(MenuEvent event) const {
    switch (event) {
        case input::MenuEvent::Pause:
            return m_settings.pause.to_string();
        case input::MenuEvent::OpenSettings:
            return m_settings.open_settings.to_string();
        default:
            UNREACHABLE();
    }
    //
}


[[nodiscard]] std::optional<InputEvent> input::ControllerGameInput::sdl_event_to_input_event(const SDL_Event& event
) const {
    if (event.type == SDL_CONTROLLERBUTTONDOWN) {

        if (event.cbutton.which != underlying_input()->instance_id()) {
            return std::nullopt;
        }

        //TODO(Totto): use switch case
        const auto button = sdl::ControllerKey{ static_cast<SDL_GameControllerButton>(event.cbutton.button) };

        if (button == m_settings.rotate_left) {
            return InputEvent::RotateLeftPressed;
        }
        if (button == m_settings.rotate_right) {
            return InputEvent::RotateRightPressed;
        }
        if (button == m_settings.move_down) {
            return InputEvent::MoveDownPressed;
        }
        if (button == m_settings.move_left) {
            return InputEvent::MoveLeftPressed;
        }
        if (button == m_settings.move_right) {
            return InputEvent::MoveRightPressed;
        }
        if (button == m_settings.drop) {
            return InputEvent::DropPressed;
        }
        if (button == m_settings.hold) {
            return InputEvent::HoldPressed;
        }
    } else if (event.type == SDL_CONTROLLERBUTTONUP) {

        if (event.cbutton.which != underlying_input()->instance_id()) {
            return std::nullopt;
        }

        const auto button = sdl::ControllerKey{ static_cast<SDL_GameControllerButton>(event.cbutton.button) };

        if (button == m_settings.rotate_left) {
            return InputEvent::RotateLeftReleased;
        }
        if (button == m_settings.rotate_right) {
            return InputEvent::RotateRightReleased;
        }
        if (button == m_settings.move_down) {
            return InputEvent::MoveDownReleased;
        }
        if (button == m_settings.move_left) {
            return InputEvent::MoveLeftReleased;
        }
        if (button == m_settings.move_right) {
            return InputEvent::MoveRightReleased;
        }
        if (button == m_settings.drop) {
            return InputEvent::DropReleased;
        }
        if (button == m_settings.hold) {
            return InputEvent::HoldReleased;
        }
    }
    return std::nullopt;
}


sdl::ControllerKey json_helper::get_controller_key(const nlohmann::json& obj, const std::string& name) {

    auto context = obj.at(name);

    std::string input;
    context.get_to(input);

    const auto& value = sdl::ControllerKey::from_string(string::to_lower_case(input));

    if (not value.has_value()) {
        throw nlohmann::json::type_error::create(
                302, fmt::format("Expected a valid Key for key '{}', but got '{}': {}", name, input, value.error()),
                &context
        );
    }
    return value.value();
}
