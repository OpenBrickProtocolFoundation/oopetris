

#include "joystick_input.hpp"
#include "helper/expected.hpp"
#include "helper/optional.hpp"
#include "helper/utils.hpp"
#include "input/game_input.hpp"
#include "input/input.hpp"

#include <exception>
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

    //TODO(Totto): add support for gamecontrollers (SDL_IsGameController)

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

    spdlog::debug("Found {} Joysticks", num_of_joysticks);

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
            for (auto it = inputs.cbegin(); it != inputs.cend(); it++) {

                if (const auto joystick_input = utils::is_child_class<input::JoystickInput>(*it);
                    joystick_input.has_value()) {

                    if (joystick_input.value()->instance_id() == instance_id) {
                        //TODO(Totto): if we use this joystick as game input we have to notify the user about it,and pause the game, until he is inserted again

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
}


#if defined(__CONSOLE__)

#if defined(__SWITCH__)

input::SwitchJoystickInput_Type1::SwitchJoystickInput_Type1(
        SDL_Joystick* joystick,
        SDL_JoystickID instance_id,
        const std::string& name
)
    : ConsoleJoystickInput{
          joystick,
          instance_id,
          name,
          //NOTE: this are not all, but atm only those, who can be checked with a SDL_JOYBUTTONDOWN event
          { { "A", JOYCON_A },
           { "B", JOYCON_B },
           { "X", JOYCON_X },
           { "Y", JOYCON_Y },
           { "L", JOYCON_L },
           { "R", JOYCON_R },
           { "ZL", JOYCON_ZL },
           { "ZR", JOYCON_ZR },
           { "PLUS", JOYCON_PLUS },
           { "MINUS", JOYCON_MINUS },
           { "DPAD_LEFT", JOYCON_DPAD_LEFT },
           { "DPAD_UP", JOYCON_DPAD_UP },
           { "DPAD_RIGHT", JOYCON_DPAD_RIGHT },
           { "DPAD_DOWN", JOYCON_DPAD_DOWN },
           { "LDPAD_LEFT", JOYCON_LDPAD_LEFT },
           { "LDPAD_UP", JOYCON_LDPAD_UP },
           { "LDPAD_RIGHT", JOYCON_LDPAD_RIGHT },
           { "LDPAD_DOWN", JOYCON_LDPAD_DOWN },
           { "RDPAD_LEFT", JOYCON_RDPAD_LEFT },
           { "RDPAD_UP", JOYCON_RDPAD_UP },
           { "RDPAD_RIGHT", JOYCON_RDPAD_RIGHT },
           { "RDPAD_DOWN", JOYCON_RDPAD_DOWN } }
} { }


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


[[nodiscard]] std::string input::SwitchJoystickInput_Type1::key_to_string(console::SettingsType key) const {
    switch (key) {
        case JOYCON_A:
            return "A";
        case JOYCON_B:
            return "B";
        case JOYCON_X:
            return "X";
        case JOYCON_Y:
            return "Y";
        case JOYCON_L:
            return "L";
        case JOYCON_R:
            return "R";
        case JOYCON_ZL:
            return "ZL";
        case JOYCON_ZR:
            return "ZR";
        case JOYCON_PLUS:
            return "PLUS";
        case JOYCON_MINUS:
            return "MINUS";
        case JOYCON_DPAD_LEFT:
            return "DPAD_LEFT";
        case JOYCON_DPAD_UP:
            return "DPAD_UP";
        case JOYCON_DPAD_RIGHT:
            return "DPAD_RIGHT";
        case JOYCON_DPAD_DOWN:
            return "DPAD_DOWN";
        case JOYCON_LDPAD_LEFT:
            return "LDPAD_LEFT";
        case JOYCON_LDPAD_UP:
            return "LDPAD_UP";
        case JOYCON_LDPAD_RIGHT:
            return "LDPAD_RIGHT";
        case JOYCON_LDPAD_DOWN:
            return "LDPAD_DOWN";
        case JOYCON_RDPAD_LEFT:
            return "RDPAD_LEFT";
        case JOYCON_RDPAD_UP:
            return "RDPAD_UP";
        case JOYCON_RDPAD_RIGHT:
            return "RDPAD_RIGHT";
        case JOYCON_RDPAD_DOWN:
            return "RDPAD_DOWN";


        default:
            utils::unreachable();
    }
}

[[nodiscard]] input::JoystickSettings input::SwitchJoystickInput_Type1::to_normal_settings(
        const AbstractJoystickSettings<input::console::SettingsType>& settings
) const {

    JoystickSettings result{};

#define X_LIST_MACRO(x) SETTINGS_TO_STRING(settings, result, key_to_string, x);

    X_LIST_OF_SETTINGS_KEYS

#undef X_LIST_MACRO

    return result;
}

[[nodiscard]] input::JoystickSettings input::SwitchJoystickInput_Type1::default_settings() const {
    const AbstractJoystickSettings<console::SettingsType> settings = //
            { .identification = JoystickIdentification{},
              .rotate_left = JOYCON_DPAD_LEFT,
              .rotate_right = JOYCON_DPAD_RIGHT,
              .move_left = JOYCON_LDPAD_LEFT,
              .move_right = JOYCON_LDPAD_RIGHT,
              .move_down = JOYCON_LDPAD_DOWN,
              .drop = JOYCON_X,
              .hold = JOYCON_B,
              .pause = JOYCON_MINUS,
              .open_settings = JOYCON_PLUS };

    return to_normal_settings(settings);
}


#elif defined(__3DS__)

input::_3DSJoystickInput_Type1::_3DSJoystickInput_Type1(
        SDL_Joystick* joystick,
        SDL_JoystickID instance_id,
        const std::string& name
)
    : ConsoleJoystickInput{
          joystick,
          instance_id,
          name,
          //NOTE: this are not all, but atm only those, who can be checked with a SDL_JOYBUTTONDOWN event
          { { "A", JOYCON_A },
           { "B", JOYCON_B },
           { "SELECT", JOYCON_SELECT },
           { "START", JOYCON_START },
           { "DPAD_RIGHT", JOYCON_DPAD_RIGHT },
           { "DPAD_LEFT", JOYCON_DPAD_LEFT },
           { "DPAD_UP", JOYCON_DPAD_UP },
           { "DPAD_DOWN", JOYCON_DPAD_DOWN },
           { "R", JOYCON_R },
           { "L", JOYCON_L },
           { "X", JOYCON_X },
           { "Y", JOYCON_Y },
           { "ZL", JOYCON_ZL },
           { "ZR", JOYCON_ZR } }
} { }


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


[[nodiscard]] std::string input::_3DSJoystickInput_Type1::key_to_string(console::SettingsType key) const {
    switch (key) {
        case JOYCON_A:
            return "A";
        case JOYCON_B:
            return "B";
        case JOYCON_SELECT:
            return "SELECT";
        case JOYCON_START:
            return "START";
        case JOYCON_DPAD_RIGHT:
            return "DPAD_RIGHT";
        case JOYCON_DPAD_LEFT:
            return "DPAD_LEFT";
        case JOYCON_DPAD_UP:
            return "DPAD_UP";
        case JOYCON_DPAD_DOWN:
            return "DPAD_DOWN";
        case JOYCON_R:
            return "R";
        case JOYCON_L:
            return "L";
        case JOYCON_X:
            return "X";
        case JOYCON_Y:
            return "Y";
        case JOYCON_ZL:
            return "ZL";
        case JOYCON_ZR:
            return "ZR";
        default:
            utils::unreachable();
    }
}

[[nodiscard]] input::JoystickSettings input::_3DSJoystickInput_Type1::to_normal_settings(
        const AbstractJoystickSettings<input::console::SettingsType>& settings
) const {

    JoystickSettings result{};

#define X_LIST_MACRO(x) SETTINGS_TO_STRING(settings, result, key_to_string, x);

    X_LIST_OF_SETTINGS_KEYS

#undef X_LIST_MACRO

    return result;
}

[[nodiscard]] input::JoystickSettings input::_3DSJoystickInput_Type1::default_settings() const {
    const AbstractJoystickSettings<console::SettingsType> settings = //
            { .identification = JoystickIdentification{},
              .rotate_left = JOYCON_L,
              .rotate_right = JOYCON_R,
              .move_left = JOYCON_DPAD_LEFT,
              .move_right = JOYCON_DPAD_RIGHT,
              .move_down = JOYCON_DPAD_DOWN,
              .drop = JOYCON_X,
              .hold = JOYCON_B,
              .pause = JOYCON_START,
              .open_settings = JOYCON_SELECT };

    return to_normal_settings(settings);
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


namespace {

    [[nodiscard]] helper::optional<std::shared_ptr<input::JoystickGameInput>> get_game_joystick_by_guid(
            const SDL::GUID& guid,
            const input::JoystickSettings& settings,
            EventDispatcher* event_dispatcher,
            input::JoystickInput* underlying_input

    ) {
#if defined(__CONSOLE__)
#if defined(__SWITCH__)
        if (guid == input::SwitchJoystickInput_Type1::guid) {
            return std::make_shared<input::ConsoleJoystickGameInput>(settings, event_dispatcher, underlying_input);
        }
#elif defined(__3DS__)
        if (guid == input::_3DSJoystickInput_Type1::guid) {
            return std::make_shared<input::ConsoleJoystickGameInput>(settings, event_dispatcher, underlying_input);
        }

#endif
#endif

        UNUSED(guid);
        UNUSED(settings);
        UNUSED(event_dispatcher);
        UNUSED(underlying_input);

        return helper::nullopt;
    }


} // namespace

[[nodiscard]] helper::expected<std::shared_ptr<input::JoystickGameInput>, std::string>
input::JoystickGameInput::get_game_input_by_settings(
        const input::InputManager& input_manager,
        EventDispatcher* event_dispatcher,
        const JoystickSettings& settings
) {


    for (const auto& input : input_manager.inputs()) {
        if (const auto joystick_input = utils::is_child_class<input::JoystickInput>(input);
            joystick_input.has_value()) {

            try {
                auto result = get_game_joystick_by_guid(
                        settings.identification.guid, settings, event_dispatcher, joystick_input.value()
                );

                if (result.has_value()) {
                    return result.value();
                }

            } catch (const std::exception& exception) {
                spdlog::warn("Couldn't construct JoystickGameInput: {}", exception.what());
            }
        }
    }

    return helper::unexpected<std::string>{
        fmt::format("No JoystickGameInput candidate for the GUID: {}", settings.identification.guid.to_string())
    };
}


#if defined(__CONSOLE__)


input::ConsoleJoystickInput::ConsoleJoystickInput(
        SDL_Joystick* joystick,
        SDL_JoystickID instance_id,
        const std::string& name,
        const MappingType<console::SettingsType>& key_mappings
)
    : JoystickInput{ joystick, instance_id, name },
      m_key_mappings{ key_mappings } { }

[[nodiscard]] const input::MappingType<input::console::SettingsType>& input::ConsoleJoystickInput::key_mappings(
) const {
    return m_key_mappings;
}

input::ConsoleJoystickGameInput::ConsoleJoystickGameInput(
        JoystickSettings settings,
        EventDispatcher* event_dispatcher,
        JoystickInput* underlying_input
)
    : JoystickGameInput{ event_dispatcher, underlying_input } {

    auto console_input = utils::is_child_class<ConsoleJoystickInput>(underlying_input);

    if (not console_input.has_value()) {
        throw std::runtime_error("Invalid input received");
    }

    m_underlying_joystick_input = console_input.value();

    auto validate_settings =
            JoystickGameInput::try_resolve_settings(settings, m_underlying_joystick_input->key_mappings());
    if (not validate_settings.has_value()) {
        throw std::runtime_error("Invalid settings");
    }

    m_settings = validate_settings.value();
}

input::ConsoleJoystickGameInput::~ConsoleJoystickGameInput() = default;

// game_input uses Input to handle events, but stores the config settings for the specific button

helper::optional<InputEvent> input::ConsoleJoystickGameInput::sdl_event_to_input_event(const SDL_Event& event) const {
    if (event.type == SDL_JOYBUTTONDOWN) {

        if (event.jbutton.which != m_underlying_input->instance_id()) {
            return helper::nullopt;
        }

        //TODO: use switch case
        const auto button = event.jbutton.button;
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
        if (button == m_settings.rotate_right) {
            return InputEvent::MoveRightPressed;
        }
        if (button == m_settings.drop) {
            return InputEvent::DropPressed;
        }
        if (button == m_settings.hold) {
            return InputEvent::HoldPressed;
        }
    } else if (event.type == SDL_JOYBUTTONUP) {

        if (event.jbutton.which != m_underlying_input->instance_id()) {
            return helper::nullopt;
        }

        const auto button = event.jbutton.button;
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
        if (button == m_settings.rotate_right) {
            return InputEvent::MoveRightReleased;
        }
        if (button == m_settings.drop) {
            return InputEvent::DropReleased;
        }
        if (button == m_settings.hold) {
            return InputEvent::HoldReleased;
        }
    }
    return helper::nullopt;
}

[[nodiscard]] helper::optional<input::MenuEvent> input::ConsoleJoystickGameInput::get_menu_event(const SDL_Event& event
) const {

    if (event.type == SDL_JOYBUTTONDOWN) {

        if (event.jbutton.which != m_underlying_input->instance_id()) {
            return helper::nullopt;
        }

        const auto button = event.jbutton.button;

        if (button == m_settings.pause) {
            return MenuEvent::PAUSE;
        }
        if (button == m_settings.open_settings) {
            return MenuEvent::OPEN_SETTINGS;
        }
    }

    return helper::nullopt;
}


[[nodiscard]] std::string input::ConsoleJoystickGameInput::describe_menu_event(MenuEvent event) const {
    switch (event) {
        case input::MenuEvent::PAUSE:
            return m_underlying_joystick_input->key_to_string(m_settings.pause);
        case input::MenuEvent::OPEN_SETTINGS:
            return m_underlying_joystick_input->key_to_string(m_settings.open_settings);
        default:
            utils::unreachable();
    }
}


#endif


std::string json_helper::get_key_from_object(const nlohmann::json& obj, const std::string& name) {

    const auto& context = obj.at(name);

    std::string input;
    context.get_to(input);


    return input;
}
