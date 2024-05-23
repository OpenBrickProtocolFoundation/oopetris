

#include "gamecontroller_input.hpp"
#include "input/input.hpp"


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


[[nodiscard]] helper::optional<input::NavigationEvent> input::ControllerInput::get_navigation_event(
        const SDL_Event& event
) const {
    if (event.type == SDL_CONTROLLERBUTTONDOWN) {

        if (event.cbutton.which != instance_id()) {
            return helper::nullopt;
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
                return helper::nullopt;

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
            utils::unreachable();
    }
}


[[nodiscard]] helper::optional<input::NavigationEvent> input::ControllerInput::handle_axis_navigation_event(
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
            return helper::nullopt;
        }

        // x axis movement
        if (event.caxis.axis == SDL_CONTROLLER_AXIS_LEFTX or event.caxis.axis == SDL_CONTROLLER_AXIS_RIGHTX) {
            if (event.caxis.value > axis_threshold) {
                return NavigationEvent::RIGHT;
            }

            if (event.caxis.value < -axis_threshold) {
                return NavigationEvent::LEFT;
            }

            return helper::nullopt;
        }

        // y axis movement
        if (event.caxis.axis == SDL_CONTROLLER_AXIS_LEFTY or event.caxis.axis == SDL_CONTROLLER_AXIS_RIGHTY) {
            if (event.caxis.value > axis_threshold) {
                return NavigationEvent::DOWN;
            }

            if (event.caxis.value < -axis_threshold) {
                return NavigationEvent::UP;
            }

            return helper::nullopt;
        }

        //Note: not all types of SDL_GameControllerAxis are handled, since they are not needed for navigation events

        return helper::nullopt;
    }

    return helper::nullopt;
}
