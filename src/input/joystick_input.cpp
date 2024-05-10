

#include "joystick_input.hpp"
#include "helper/expected.hpp"
#include "helper/optional.hpp"
#include "helper/utils.hpp"
#include "input/console_buttons.hpp"
#include "input/game_input.hpp"
#include "input/input.hpp"

#include <spdlog/spdlog.h>


input::JoystickInput::JoystickInput(SDL_Joystick* joystick, SDL_JoystickID instance_id, const std::string& name)
    : input::Input{ name, input::InputType::JoyStick },
      m_joystick{ joystick },
      m_instance_id{ instance_id } { }


input::JoystickInput::~JoystickInput() {
    SDL_JoystickClose(m_joystick);
}


[[nodiscard]] helper::optional<std::unique_ptr<input::JoystickInput>> input::JoystickInput::get_joystick_by_guid(
        const SDL::GUID& guid,
        SDL_Joystick* joystick,
        SDL_JoystickID instance_id,
        const std::string& name

) {
#if defined(__CONSOLE__)
#if defined(__SWITCH__)
    if (guid == SwitchJoystickInput_Type1::guid) {
        return std::make_unique<SwitchJoystickInput_Type1>(joystick, instance_id, name);
    }
#elif defined(__3DS__)
    if (guid == _3DSJoystickInput_Type1::guid) {
        return std::make_unique<_3DSJoystickInput_Type1>(joystick, instance_id, name);
    }

#endif
#endif

    UNUSED(guid);
    UNUSED(joystick);
    UNUSED(instance_id);
    UNUSED(name);

    return helper::nullopt;
}


[[nodiscard]] helper::expected<std::unique_ptr<input::JoystickInput>, std::string>
input::JoystickInput::get_by_device_index(int device_index) {

    auto* joystick = SDL_JoystickOpen(device_index);

    if (joystick == nullptr) {
        return helper::unexpected<std::string>{
            fmt::format("Failed to get joystick at device index {}: {}", device_index, SDL_GetError())
        };
    }

    //TODO: add support for gamecontrollers (SDL_IsGameController)

    const auto instance_id = SDL_JoystickInstanceID(joystick);

    if (instance_id < 0) {
        return helper::unexpected<std::string>{ fmt::format("Failed to get joystick instance id: {}", SDL_GetError()) };
    }

    std::string name = "unknown name";
    const auto* char_name = SDL_JoystickName(joystick);

    if (char_name != nullptr) {
        name = char_name;
    }


    const auto guid = SDL::GUID{ SDL_JoystickGetGUID(joystick) };

    if (guid == SDL::GUID{}) {
        return helper::unexpected<std::string>{ fmt::format("Failed to get joystick GUID: {}", SDL_GetError()) };
    }

    auto joystick_input = JoystickInput::get_joystick_by_guid(guid, joystick, instance_id, name);

    if (joystick_input.has_value()) {
        return std::move(joystick_input.value());
    }

    return helper::unexpected<std::string>{ fmt::format(
            "Failed to get joystick model by GUID {} We don't support this joystick yet: the name was {}", guid, name
    ) };
}

[[nodiscard]] SDL_JoystickID input::JoystickInput::instance_id() const {
    return m_instance_id;
}

void input::JoyStickInputManager::discover_devices(std::vector<std::unique_ptr<Input>>& inputs) {

    //initialize joystick input, this needs to call some sdl things

    const auto result = SDL_InitSubSystem(SDL_INIT_JOYSTICK);

    if (result != 0) {
        spdlog::warn("Failed to initialize the joystick system: {}", SDL_GetError());
        return;
    }


    const auto enable_result = SDL_JoystickEventState(SDL_ENABLE);

    if (enable_result != 1) {
        const auto* const error = enable_result == 0 ? "it was disabled" : SDL_GetError();
        spdlog::warn("Failed to set JoystickEventState (automatic polling by SDL): {}", error);

        return;
    }


    const auto allow_background_events_result = SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");

    if (allow_background_events_result != SDL_TRUE) {
        spdlog::warn("Failed to set the JOYSTICK_ALLOW_BACKGROUND_EVENTS hint: {}", SDL_GetError());

        return;
    }


    const auto num_of_joysticks = SDL_NumJoysticks();

    if (num_of_joysticks < 0) {
        spdlog::warn("Failed to get number of joysticks: {}", SDL_GetError());
        return;
    }

    for (auto i = 0; i < num_of_joysticks; ++i) {

        auto joystick = JoystickInput::get_by_device_index(i);
        if (joystick.has_value()) {
            inputs.push_back(std::move(joystick.value()));
        } else {
            spdlog::warn("Failed to configure joystick: {}", joystick.error());
        }
    }
}


[[nodiscard]] bool input::JoyStickInputManager::process_special_inputs(
        const SDL_Event& event,
        std::vector<std::unique_ptr<Input>>& inputs
) {

    switch (event.type) {
        case SDL_JOYDEVICEADDED: {
            const auto device_id = event.jdevice.which;
            auto joystick = JoystickInput::get_by_device_index(device_id);
            if (joystick.has_value()) {
                inputs.push_back(std::move(joystick.value()));
            } else {
                spdlog::warn("Failed to add newly added joystick: {}", joystick.error());
            }
            return true;
        }
        case SDL_JOYDEVICEREMOVED: {
            const auto instance_id = event.jdevice.which;
            for (auto it = inputs.cbegin(); it != inputs.end(); it++) {

                if (const auto joystick_input = utils::is_child_class<input::JoystickInput>(*it);
                    joystick_input.has_value()) {

                    if (joystick_input.value()->instance_id() == instance_id) {
                        inputs.erase(it);
                        return true;
                    }
                }
            }

            spdlog::warn("Failed to remove removed joystick from internal joystick vector");

            return true;
        }
        default:
            return false;
    }
    //TODO
}


#if defined(__CONSOLE__)

#if defined(__SWITCH__)

input::SwitchJoystickInput_Type1::SwitchJoystickInput_Type1(
        SDL_Joystick* joystick,
        SDL_JoystickID instance_id,
        const std::string& name
)
    : JoystickInput{ joystick, instance_id, name } { }


[[nodiscard]] helper::optional<input::NavigationEvent> input::SwitchJoystickInput_Type1::get_navigation_event(
        const SDL_Event& event
) const {

    if (event.type == SDL_JOYBUTTONDOWN) {

        if (event.jbutton.which != instance_id()) {
            return helper::nullopt;
        }

        switch (event.jbutton.button) {
            case JOYCON_A:
                return NavigationEvent::OK;
            case JOYCON_DPAD_DOWN:
            case JOYCON_LDPAD_DOWN:
            case JOYCON_RDPAD_DOWN:
                return NavigationEvent::DOWN;
            case JOYCON_DPAD_UP:
            case JOYCON_LDPAD_UP:
            case JOYCON_RDPAD_UP:
                return NavigationEvent::UP;
            case JOYCON_DPAD_LEFT:
            case JOYCON_LDPAD_LEFT:
            case JOYCON_RDPAD_LEFT:
                return NavigationEvent::LEFT;
            case JOYCON_DPAD_RIGHT:
            case JOYCON_LDPAD_RIGHT:
            case JOYCON_RDPAD_RIGHT:
                return NavigationEvent::RIGHT;
            case JOYCON_MINUS:
                return NavigationEvent::BACK;
            default:
                return helper::nullopt;

                //note, that  NavigationEvent::TAB is not supported
        }
    }

    //TODO: handle SDL_JOYAXISMOTION


    return helper::nullopt;
}

[[nodiscard]] std::string input::SwitchJoystickInput_Type1::describe_navigation_event(NavigationEvent event) const {
    switch (event) {
        case NavigationEvent::OK:
            return "A";
        case NavigationEvent::BACK:
            return "Minus";
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


#elif defined(__3DS__)

input::_3DSJoystickInput_Type1::_3DSJoystickInput_Type1(
        SDL_Joystick* joystick,
        SDL_JoystickID instance_id,
        const std::string& name
)
    : JoystickInput{ joystick, instance_id, name } { }


[[nodiscard]] helper::optional<input::NavigationEvent> input::_3DSJoystickInput_Type1::get_navigation_event(
        const SDL_Event& event
) const {


    if (event.type == SDL_JOYBUTTONDOWN) {

        if (event.jbutton.which != instance_id()) {
            return helper::nullopt;
        }

        switch (event.jbutton.button) {
            case JOYCON_A:
                return NavigationEvent::OK;
            case JOYCON_DPAD_DOWN:
            case JOYCON_CSTICK_DOWN:
            case JOYCON_CPAD_DOWN:
                return NavigationEvent::DOWN;
            case JOYCON_DPAD_UP:
            case JOYCON_CSTICK_UP:
            case JOYCON_CPAD_UP:
                return NavigationEvent::UP;
            case JOYCON_DPAD_LEFT:
            case JOYCON_CSTICK_LEFT:
            case JOYCON_CPAD_LEFT:
                return NavigationEvent::LEFT;
            case JOYCON_DPAD_RIGHT:
            case JOYCON_CSTICK_RIGHT:
            case JOYCON_CPAD_RIGHT:
                return NavigationEvent::RIGHT;
            case JOYCON_X:
                return NavigationEvent::BACK;
            default:
                return helper::nullopt;

                //note, that  NavigationEvent::TAB is not supported
        }
    }

    //TODO: handle SDL_JOYAXISMOTION


    return helper::nullopt;
}

[[nodiscard]] std::string input::_3DSJoystickInput_Type1::describe_navigation_event(NavigationEvent event) const {
    switch (event) {
        case NavigationEvent::OK:
            return "A";
        case NavigationEvent::BACK:
            return "X";
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


#endif
#endif

void input::JoystickGameInput::handle_event(const SDL_Event& event) {
    m_event_buffer.push_back(event);
}

void input::JoystickGameInput::update(SimulationStep simulation_step_index) {
    for (const auto& event : m_event_buffer) {
        const auto input_event = sdl_event_to_input_event(event);
        if (input_event.has_value()) {
            GameInput::handle_event(*input_event, simulation_step_index);
        }
    }
    m_event_buffer.clear();

    GameInput::update(simulation_step_index);
}


[[nodiscard]] helper::optional<std::shared_ptr<input::JoystickGameInput>>
input::JoystickGameInput::get_game_input_by_settings(
        const input::InputManager& input_manager,
        EventDispatcher* event_dispatcher,
        const JoystickSettings& settings
) {

    //TODO: filter all input_manager.inputs() by guid, than try to get them in order, by trying to convert the settings validate them and create the game input

    //TODO: set up: that on removal of the joystick_input pause is pressed and either a new gam,einput is created or waited until he reconnects!


    UNUSED(input_manager);
    UNUSED(settings);
    UNUSED(event_dispatcher);
    UNUSED(settings);

    return helper::nullopt;
}


#if defined(__CONSOLE__)
#if defined(__SWITCH__)


// game_input uses Input to handle events, but stores the config settings for the specific button

//TODO: use settings
helper::optional<InputEvent> input::SwitchJoystickGameInput_Type1::sdl_event_to_input_event(const SDL_Event& event
) const {
    if (event.type == SDL_JOYBUTTONDOWN) {

        //TODO
        /* if (event.jbutton.which != m_instance_id) {
            return helper::nullopt;
        } */

        //TODO: use switch case
        const auto button = event.jbutton.button;
        if (button == JOYCON_DPAD_LEFT) {
            return InputEvent::RotateLeftPressed;
        }
        if (button == JOYCON_DPAD_RIGHT) {
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

        //TODO
        /*  if (event.jbutton.which != m_instance_id) {
            return helper::nullopt;
        } */

        const auto button = event.jbutton.button;
        if (button == JOYCON_DPAD_LEFT) {
            return InputEvent::RotateLeftReleased;
        }
        if (button == JOYCON_DPAD_RIGHT) {
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
    return helper::nullopt;
}
#elif defined(__3DS__)

//TODO: use settings
helper::optional<InputEvent> input::_3DSJoystickGameInput_Type1::sdl_event_to_input_event(const SDL_Event& event
) const {
    if (event.type == SDL_JOYBUTTONDOWN) {

        //TODO:
        /*   if (event.jbutton.which != m_instance_id) {
            return helper::nullopt;
        } */

        const auto button = event.jbutton.button;
        if (button == JOYCON_L) {
            return InputEvent::RotateLeftPressed;
        }
        if (button == JOYCON_R) {
            return InputEvent::RotateRightPressed;
        }
        if (button == JOYCON_DPAD_DOWN or button == JOYCON_CSTICK_DOWN) {
            return InputEvent::MoveDownPressed;
        }
        if (button == JOYCON_DPAD_LEFT or button == JOYCON_CSTICK_LEFT) {
            return InputEvent::MoveLeftPressed;
        }
        if (button == JOYCON_DPAD_RIGHT or button == JOYCON_CSTICK_RIGHT) {
            return InputEvent::MoveRightPressed;
        }
        if (button == JOYCON_A) {
            return InputEvent::DropPressed;
        }
        if (button == JOYCON_B) {
            return InputEvent::HoldPressed;
        }
    } else if (event.type == SDL_JOYBUTTONUP) {

        //TODO:
        /* if (event.jbutton.which != m_instance_id) {
            return helper::nullopt;
        } */

        const auto button = event.jbutton.button;
        if (button == JOYCON_L) {
            return InputEvent::RotateLeftReleased;
        }
        if (button == JOYCON_R) {
            return InputEvent::RotateRightReleased;
        }
        if (button == JOYCON_DPAD_DOWN or button == JOYCON_CSTICK_DOWN) {
            return InputEvent::MoveDownReleased;
        }
        if (button == JOYCON_DPAD_LEFT or button == JOYCON_CSTICK_LEFT) {
            return InputEvent::MoveLeftReleased;
        }
        if (button == JOYCON_DPAD_RIGHT or button == JOYCON_CSTICK_RIGHT) {
            return InputEvent::MoveRightReleased;
        }
        if (button == JOYCON_A) {
            return InputEvent::DropReleased;
        }
        if (button == JOYCON_B) {
            return InputEvent::HoldReleased;
        }
    }
    return helper::nullopt;
}
#endif


#endif


std::string json_helper::get_key_from_object(const nlohmann::json& j, const std::string& name) {

    const auto& context = j.at(name);

    std::string input;
    context.get_to(input);


    return input;
}
