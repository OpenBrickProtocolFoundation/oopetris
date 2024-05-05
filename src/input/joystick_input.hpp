#pragma once


#include "SDL_joystick.h"
#include "helper/expected.hpp"
#include "helper/parse_json.hpp"
#include "input.hpp"
#include "input/game_input.hpp"
#include "manager/event_dispatcher.hpp"

#include <fmt/format.h>
#include <string>

namespace joystick {
    struct GUID {
    private:
        std::array<u8, 16> m_guid;

    public:
        GUID();
        GUID(const SDL_GUID& data);

        static helper::expected<GUID, std::string> from_string(const std::string& value);

        [[nodiscard]] bool operator==(const GUID& other) const;

        [[nodiscard]] operator std::string() const;
    };
} // namespace joystick


template<>
struct fmt::formatter<joystick::GUID> : formatter<std::string> {
    auto format(const joystick::GUID& guid, format_context& ctx) {
        return formatter<std::string>::format(std::string{ guid }, ctx);
    }
};

namespace input {

    struct JoystickInput : Input {
    private:
        SDL_Joystick* m_joystick;
        SDL_JoystickID m_instance_id;

        [[nodiscard]] static helper::optional<std::unique_ptr<JoystickInput>>
        get_joystick_by_guid(const joystick::GUID& guid, SDL_Joystick* joystick, SDL_JoystickID instance_id);

    public:
        JoystickInput(SDL_Joystick* joystick, SDL_JoystickID instance_id, const std::string& name);
        virtual ~JoystickInput();

        [[nodiscard]] static helper::expected<std::unique_ptr<JoystickInput>, std::string> get_by_device_index(
                int device_index
        );
        [[nodiscard]] SDL_JoystickID instance_id() const;
    };


    //TODO: also support gamecontroller API
    // see: https://github.com/mdqinc/SDL_GameControllerDB?tab=readme-ov-file

    struct JoyStickInputManager {
    private:
        std::vector<std::unique_ptr<JoystickInput>> m_inputs{};

    public:
        explicit JoyStickInputManager();
        [[nodiscard]] const std::vector<std::unique_ptr<JoystickInput>>& inputs() const;

        [[nodiscard]] helper::optional<NavigationEvent> get_navigation_event(const SDL_Event& event) const;

        [[nodiscard]] helper::optional<PointerEventHelper> get_pointer_event(const SDL_Event& event) const;

        [[nodiscard]] bool process_special_inputs(const SDL_Event& event);
    };


    // essentially a GUID
    struct JoystickIdentification {
        joystick::GUID guid;


        static helper::expected<JoystickIdentification, std::string> from_string(const std::string& value);
    };

    //using std::string in here, since we only know, if these are valid joystick button names, after parsing the GUID and than seeing if we support that joystick and than using the string mappings for that specific joystick
    struct JoystickSettings : InputSettings {
        JoystickIdentification identification;

        std::string rotate_left{};
        std::string rotate_right{};
        std::string move_left{};
        std::string move_right{};
        std::string move_down{};
        std::string drop{};
        std::string hold{};

        std::string pause{};
        std::string open_settings{};


        [[nodiscard]] helper::expected<bool, std::string> validate() const override;
    };


    //TODO: differntiate different controllers and modes, e.g the switch can have pro controller, the included ones, each of them seperate etc.

#if defined(__CONSOLE__)
#if defined(__SWITCH__)
    struct SwitchJoystickInput_Type1 : JoystickInput {

        //TODO
        static joystick::GUID guid{};

    public:
        SwitchJoystickInput_Type1(SDL_Joystick* joystick, SDL_JoystickID instance_id, const std::string& name);

        [[nodiscard]] helper::optional<NavigationEvent> get_navigation_event(const SDL_Event& event) const override;
    };


#elif defined(__3DS__)

    struct _3DSJoystickInput_Type1 : JoystickInput {

        //TODO
        static joystick::GUID guid{};

    public:
        _3DSJoystickInput_Type1(SDL_Joystick* joystick, SDL_JoystickID instance_id, const std::string& name);

        [[nodiscard]] helper::optional<NavigationEvent> get_navigation_event(const SDL_Event& event) const override;
    };


#endif

#endif

    struct JoystickGameInput : public GameInput, public EventListener {
    private:
        JoystickSettings m_settings;
        std::vector<SDL_Event> m_event_buffer;
        EventDispatcher* m_event_dispatcher;

    public:
        JoystickGameInput(const JoystickSettings& settings, EventDispatcher* event_dispatcher)
            : GameInput{ GameInputType::Controller },
              m_settings{ settings },
              m_event_dispatcher{ event_dispatcher } {
            m_event_dispatcher->register_listener(this);
        }

        ~JoystickGameInput() override {
            m_event_dispatcher->unregister_listener(this);
        }

        void handle_event(const SDL_Event& event) override;

        void update(SimulationStep simulation_step_index) override;

        [[nodiscard]] helper::optional<MenuEvent> get_menu_event(const SDL_Event& event) const override;

        [[nodiscard]] std::string describe_menu_event(MenuEvent event) const override;

    private:
        [[nodiscard]] helper::optional<InputEvent> sdl_event_to_input_event(const SDL_Event& event) const;
    };

} // namespace input


inline void to_json(nlohmann::json& j, const input::JoystickIdentification& identification) {

    j = nlohmann::json{
        { "guid", std::string{ identification.guid } },
    };
}


inline void to_json(nlohmann::json& j, const input::JoystickSettings& settings) {

    auto identification = nlohmann::json{};
    to_json(identification, settings.identification);

    j = nlohmann::json{
        { "identification", identification },
        { "rotate_left", settings.rotate_left },
        { "rotate_right", settings.rotate_right },
        { "move_left", settings.move_left },
        { "move_right", settings.move_right },
        { "move_down", settings.move_down },
        { "drop", settings.drop },
        { "hold", settings.hold },
        {
         "menu", nlohmann::json{
                        { "pause", settings.pause },
                        { "open_settings", settings.open_settings },
                }, }
    };
}

namespace json_helper {


    [[nodiscard]] std::string get_key_from_object(const nlohmann::json& j, const std::string& name);

} // namespace json_helper


inline void from_json(const nlohmann::json& j, input::JoystickIdentification& identification) {

    json::check_for_no_additional_keys(j, { "guid" });

    auto context = j.at("guid");

    std::string input;
    context.get_to(input);

    const auto& value = joystick::GUID::from_string(input);

    if (not value.has_value()) {
        throw nlohmann::json::type_error::create(
                302, fmt::format("Expected a valid GUID but got '{}': {}", input, value.error()), &context
        );
    }

    identification.guid = value.value();
}


inline void from_json(const nlohmann::json& j, input::JoystickSettings& settings) {

    json::check_for_no_additional_keys(
            j, { "type", "identification", "rotate_left", "rotate_right", "move_left", "move_right", "move_down",
                 "drop", "hold", "menu" }
    );

    input::JoystickIdentification identification{ joystick::GUID{} };

    from_json(j.at("identification"), identification);
    settings.identification = std::move(identification);

    settings.rotate_left = json_helper::get_key_from_object(j, "rotate_left");
    settings.rotate_right = json_helper::get_key_from_object(j, "rotate_right");
    settings.move_left = json_helper::get_key_from_object(j, "move_left");
    settings.move_right = json_helper::get_key_from_object(j, "move_right");
    settings.move_down = json_helper::get_key_from_object(j, "move_down");
    settings.drop = json_helper::get_key_from_object(j, "drop");
    settings.hold = json_helper::get_key_from_object(j, "hold");

    const auto& menu = j.at("menu");

    json::check_for_no_additional_keys(menu, { "pause", "open_settings" });

    settings.pause = json_helper::get_key_from_object(menu, "pause");
    settings.open_settings = json_helper::get_key_from_object(menu, "open_settings");

    const auto is_valid = settings.validate();
    if (not is_valid.has_value()) {
        throw std::runtime_error(is_valid.error());
    }
}


//TODO:
/* 
#elif defined(__SWITCH__)
    switch (action) {
        case CrossPlatformAction::OK:
            return "A";
        case CrossPlatformAction::PAUSE:
        case CrossPlatformAction::UNPAUSE:
            return "PLUS";
        case CrossPlatformAction::CLOSE:
        case CrossPlatformAction::EXIT:
            return "MINUS";
        case CrossPlatformAction::DOWN:
            return "Down";
        case CrossPlatformAction::UP:
            return "Up";
        case CrossPlatformAction::LEFT:
            return "Left";
        case CrossPlatformAction::RIGHT:
            return "Right";
        case CrossPlatformAction::OPEN_SETTINGS:
            return "Y";
        default:
            utils::unreachable();
    }

#elif defined(__3DS__)
    switch (action) {
        case CrossPlatformAction::OK:
            return "A";
        case CrossPlatformAction::PAUSE:
        case CrossPlatformAction::UNPAUSE:
            return "Y";
        case CrossPlatformAction::CLOSE:
        case CrossPlatformAction::EXIT:
            return "X";
        case CrossPlatformAction::DOWN:
            return "Down";
        case CrossPlatformAction::UP:
            return "Up";
        case CrossPlatformAction::LEFT:
            return "Left";
        case CrossPlatformAction::RIGHT:
            return "Right";
        case CrossPlatformAction::OPEN_SETTINGS:
            return "Select";
        default:
            utils::unreachable();
    } */