#pragma once


#include "guid.hpp"
#include "helper/expected.hpp"
#include "helper/parse_json.hpp"
#include "input.hpp"
#include "input/console_buttons.hpp"
#include "input/game_input.hpp"
#include "manager/event_dispatcher.hpp"

#include <SDL.h>
#include <fmt/format.h>
#include <string>
#include <unordered_map>

namespace input {


    /**
 * @brief 
 *
 * @note regarding the NOLINT: the destructor just cleans up the SDL_Joystick, it has nothing to do with class members that would need special member functions to be explicitly defined
 * 
 */
    struct JoystickInput //NOLINT(cppcoreguidelines-special-member-functions)
        : Input {
    private:
        SDL_Joystick* m_joystick;
        SDL_JoystickID m_instance_id;

        [[nodiscard]] static helper::optional<std::unique_ptr<JoystickInput>> get_joystick_by_guid(
                const SDL::GUID& guid,
                SDL_Joystick* joystick,
                SDL_JoystickID instance_id,
                const std::string& name
        );

    public:
        JoystickInput(SDL_Joystick* joystick, SDL_JoystickID instance_id, const std::string& name);
        ~JoystickInput();

        [[nodiscard]] static helper::expected<std::unique_ptr<JoystickInput>, std::string> get_by_device_index(
                int device_index
        );

        [[nodiscard]] SDL_JoystickID instance_id() const;

        // Add get_game_input method!
    };


    //TODO: also support gamecontroller API
    // see: https://github.com/mdqinc/SDL_GameControllerDB?tab=readme-ov-file

    struct JoyStickInputManager {
        static void discover_devices(std::vector<std::unique_ptr<Input>>& inputs);

        [[nodiscard]] static bool
        process_special_inputs(const SDL_Event& event, std::vector<std::unique_ptr<Input>>& inputs);
    };


    // essentially a GUID
    struct JoystickIdentification {
        SDL::GUID guid;

        static helper::expected<JoystickIdentification, std::string> from_string(const std::string& value);
    };

    //using std::string in here, since we only know, if these are valid joystick button names, after parsing the GUID and than seeing if we support that joystick and than using the string mappings for that specific joystick

    template<typename T>
    struct AbstractJoystickSettings {
        JoystickIdentification identification;

        T rotate_left;
        T rotate_right;
        T move_left;
        T move_right;
        T move_down;
        T drop;
        T hold;

        T pause;
        T open_settings;


        [[nodiscard]] helper::expected<bool, std::string> validate() const {
            const std::vector<std::string> to_use{ rotate_left, rotate_right, move_left, move_right,   move_down,
                                                   drop,        hold,         pause,     open_settings };

            return input::InputSettings::has_unique_members(to_use);
        }
    };

    using JoystickSettings = AbstractJoystickSettings<std::string>;


    //TODO: differntiate different controllers and modes, e.g the switch can have pro controller, the included ones, each of them seperate etc.

#if defined(__CONSOLE__)
#if defined(__SWITCH__)
    struct SwitchJoystickInput_Type1 : JoystickInput {

        //TODO
        static constexpr SDL::GUID guid{};

    public:
        SwitchJoystickInput_Type1(SDL_Joystick* joystick, SDL_JoystickID instance_id, const std::string& name);

        [[nodiscard]] helper::optional<NavigationEvent> get_navigation_event(const SDL_Event& event) const override;

        [[nodiscard]] std::string describe_navigation_event(NavigationEvent event) const override;
    };


#elif defined(__3DS__)

    struct _3DSJoystickInput_Type1 : JoystickInput {

        //TODO
        static constexpr SDL::GUID guid{};

    public:
        _3DSJoystickInput_Type1(SDL_Joystick* joystick, SDL_JoystickID instance_id, const std::string& name);

        [[nodiscard]] helper::optional<NavigationEvent> get_navigation_event(const SDL_Event& event) const override;

        [[nodiscard]] std::string describe_navigation_event(NavigationEvent event) const override;
    };


#endif

#endif

#define TRY_CONVERT(original, target, map, key)       \
    do /*NOLINT(cppcoreguidelines-avoid-do-while)*/ { \
        if (map.contains(original.key)) {             \
            target.key = map.at(original.key);        \
        } else {                                      \
            return helper::nullopt;                   \
        }                                             \
    } while (false)


#define SETTINGS_TO_STRING(original, target, fn, key) \
    do /*NOLINT(cppcoreguidelines-avoid-do-while)*/ { \
        target.key = fn(original.key);                \
    } while (false)


    struct JoystickGameInput : public GameInput, public EventListener {
    private:
        std::vector<SDL_Event> m_event_buffer;
        EventDispatcher* m_event_dispatcher;

    protected:
        JoystickInput* m_underlying_input;

    public:
        JoystickGameInput(EventDispatcher* event_dispatcher, JoystickInput* underlying_input)
            : GameInput{ GameInputType::Controller },
              m_event_dispatcher{ event_dispatcher },
              m_underlying_input{ underlying_input } {
            m_event_dispatcher->register_listener(this);
        }

        ~JoystickGameInput() override {
            m_event_dispatcher->unregister_listener(this);
        }

        void handle_event(const SDL_Event& event) override;

        void update(SimulationStep simulation_step_index) override;

        [[nodiscard]] static helper::expected<std::shared_ptr<input::JoystickGameInput>, std::string>
        get_game_input_by_settings(
                const input::InputManager& input_manager,
                EventDispatcher* event_dispatcher,
                const JoystickSettings& settings
        );

    protected:
        [[nodiscard]] virtual helper::optional<InputEvent> sdl_event_to_input_event(const SDL_Event& event) const = 0;

        template<typename T>
        using MappingType = std::unordered_map<std::string, T>;

        template<typename T>
        [[nodiscard]] static helper::optional<AbstractJoystickSettings<T>>
        try_resolve_settings(const JoystickSettings& settings, const MappingType<T>& map) {


            AbstractJoystickSettings<T> result{};

            TRY_CONVERT(settings, result, map, rotate_left);
            TRY_CONVERT(settings, result, map, rotate_right);
            TRY_CONVERT(settings, result, map, move_left);
            TRY_CONVERT(settings, result, map, move_right);
            TRY_CONVERT(settings, result, map, move_down);

            TRY_CONVERT(settings, result, map, drop);
            TRY_CONVERT(settings, result, map, hold);

            TRY_CONVERT(settings, result, map, pause);
            TRY_CONVERT(settings, result, map, open_settings);

            return result;
        }
    };


#if defined(__CONSOLE__)
#if defined(__SWITCH__)

    struct SwitchJoystickGameInput_Type1 : public JoystickGameInput {
    private:
        using SettingsType = enum JOYCON;
        AbstractJoystickSettings<SettingsType> m_settings;

        MappingType<SettingsType> m_key_mappings;

    public:
        SwitchJoystickGameInput_Type1(
                JoystickSettings settings,
                EventDispatcher* event_dispatcher,
                JoystickInput* underlying_input
        );

        virtual ~SwitchJoystickGameInput_Type1();

        [[nodiscard]] helper::optional<MenuEvent> get_menu_event(const SDL_Event& event) const override;

        [[nodiscard]] std::string describe_menu_event(MenuEvent event) const override;

    protected:
        [[nodiscard]] helper::optional<InputEvent> sdl_event_to_input_event(const SDL_Event& event) const override;

    private:
        [[nodiscard]] static std::string key_to_string(SettingsType key);

        [[nodiscard]] static JoystickSettings to_normal_settings(const AbstractJoystickSettings<SettingsType>& settings
        );
    };

#elif defined(__3DS__)
    struct _3DSJoystickGameInput_Type1 : public JoystickGameInput {
        AbstractJoystickSettings<enum JOYCON> m_settings;

    public:
        _3DSJoystickGameInput_Type1(
                JoystickSettings settings,
                EventDispatcher* event_dispatcher,
                JoystickInput* underlying_input
        );

        [[nodiscard]] helper::optional<MenuEvent> get_menu_event(const SDL_Event& event) const override;

        [[nodiscard]] std::string describe_menu_event(MenuEvent event) const override;

    protected:
        [[nodiscard]] helper::optional<InputEvent> sdl_event_to_input_event(const SDL_Event& event) const override;
    };

#endif
#endif

} // namespace input


namespace json_helper {


    [[nodiscard]] std::string get_key_from_object(const nlohmann::json& obj, const std::string& name);

} // namespace json_helper


namespace nlohmann {

    template<>
    struct adl_serializer<input::JoystickIdentification> {
        static input::JoystickIdentification from_json(const json& j) {

            ::json::check_for_no_additional_keys(j, { "guid" });

            auto context = j.at("guid");

            std::string input;
            context.get_to(input);

            const auto& value = SDL::GUID::from_string(input);

            if (not value.has_value()) {
                throw nlohmann::json::type_error::create(
                        302, fmt::format("Expected a valid GUID but got '{}': {}", input, value.error()), &context
                );
            }

            return input::JoystickIdentification{ .guid = value.value() };
        }

        static void to_json(json& j, const input::JoystickIdentification& identification) {
            j = nlohmann::json{
                { "guid", identification.guid.to_string() },
            };
        }
    };

    template<>
    struct adl_serializer<input::JoystickSettings> {
        static input::JoystickSettings from_json(const json& obj) {

            ::json::check_for_no_additional_keys(
                    obj, { "type", "identification", "rotate_left", "rotate_right", "move_left", "move_right",
                           "move_down", "drop", "hold", "menu" }
            );

            input::JoystickIdentification identification =
                    adl_serializer<input::JoystickIdentification>::from_json(obj.at("identification"));

            const auto rotate_left = json_helper::get_key_from_object(obj, "rotate_left");
            const auto rotate_right = json_helper::get_key_from_object(obj, "rotate_right");
            const auto move_left = json_helper::get_key_from_object(obj, "move_left");
            const auto move_right = json_helper::get_key_from_object(obj, "move_right");
            const auto move_down = json_helper::get_key_from_object(obj, "move_down");
            const auto drop = json_helper::get_key_from_object(obj, "drop");
            const auto hold = json_helper::get_key_from_object(obj, "hold");

            const auto& menu = obj.at("menu");

            ::json::check_for_no_additional_keys(menu, { "pause", "open_settings" });

            const auto pause = json_helper::get_key_from_object(menu, "pause");
            const auto open_settings = json_helper::get_key_from_object(menu, "open_settings");

            auto settings = input::JoystickSettings{ .identification = identification,
                                                     .rotate_left = rotate_left,
                                                     .rotate_right = rotate_right,
                                                     .move_left = move_left,
                                                     .move_right = move_right,
                                                     .move_down = move_down,
                                                     .drop = drop,
                                                     .hold = hold,
                                                     .pause = pause,
                                                     .open_settings = open_settings };

            const auto is_valid = settings.validate();
            if (not is_valid.has_value()) {
                throw std::runtime_error(is_valid.error());
            }

            return settings;
        }

        static void to_json(json& obj, const input::JoystickSettings& settings) {

            auto identification = nlohmann::json{};
            adl_serializer<input::JoystickIdentification>::to_json(identification, settings.identification);

            obj = nlohmann::json{
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
    };
} // namespace nlohmann
