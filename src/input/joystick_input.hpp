#pragma once

#include <core/helper/expected.hpp>
#include <core/helper/parse_json.hpp>

#include "guid.hpp"
#include "helper/windows.hpp"
#include "input.hpp"
#include "input/console_buttons.hpp"
#include "input/game_input.hpp"
#include "manager/event_dispatcher.hpp"

#include <SDL.h>
#include <fmt/format.h>
#include <string>
#include <unordered_map>

namespace input {


    // essentially a GUID
    struct JoystickIdentification {
        sdl::GUID guid;

        std::string name; //optional (can be ""), just for human readable settings

        OOPETRIS_GRAPHICS_EXPORTED static helper::expected<JoystickIdentification, std::string> from_string(
                const std::string& value
        );
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


        [[nodiscard]] helper::expected<void, std::string> validate() const {
            const std::vector<std::string> to_use{ rotate_left, rotate_right, move_left, move_right,   move_down,
                                                   drop,        hold,         pause,     open_settings };

            return input::InputSettings::has_unique_members(to_use);
        }
    };

    using JoystickSettings = AbstractJoystickSettings<std::string>;


    enum class JoystickLikeType : u8 { Joystick, Controller };


    struct JoystickLikeInput : Input {
    private:
        SDL_JoystickID m_instance_id;

    public:
        OOPETRIS_GRAPHICS_EXPORTED
        JoystickLikeInput(SDL_JoystickID instance_id, const std::string& name, JoystickLikeType type);

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] SDL_JoystickID instance_id() const;
    };


    struct JoystickInput : JoystickLikeInput {
    private:
        SDL_Joystick* m_joystick;

    public:
        OOPETRIS_GRAPHICS_EXPORTED
        JoystickInput(SDL_Joystick* joystick, SDL_JoystickID instance_id, const std::string& name);

        OOPETRIS_GRAPHICS_EXPORTED ~JoystickInput() override;

        OOPETRIS_GRAPHICS_EXPORTED JoystickInput(const JoystickInput& input) noexcept;
        OOPETRIS_GRAPHICS_EXPORTED JoystickInput& operator=(const JoystickInput& input) noexcept;

        OOPETRIS_GRAPHICS_EXPORTED JoystickInput(JoystickInput&& input) noexcept;
        OOPETRIS_GRAPHICS_EXPORTED JoystickInput& operator=(JoystickInput&& input) noexcept;

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] sdl::GUID guid() const;

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] virtual JoystickSettings default_settings() const = 0;

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] static std::optional<std::unique_ptr<JoystickInput>>
        get_joystick_by_guid(
                const sdl::GUID& guid,
                SDL_Joystick* joystick,
                SDL_JoystickID instance_id,
                const std::string& name
        );
    };

    struct JoyStickInputManager {
        OOPETRIS_GRAPHICS_EXPORTED static void discover_devices(std::vector<std::unique_ptr<Input>>& inputs);

        OOPETRIS_GRAPHICS_EXPORTED
        [[nodiscard]] static helper::expected<std::unique_ptr<JoystickLikeInput>, std::string> get_by_device_index(
                int device_index
        );

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] static bool
        process_special_inputs(const SDL_Event& event, std::vector<std::unique_ptr<Input>>& inputs);

    private:
        static void add_new_device(i32 device_id, std::vector<std::unique_ptr<Input>>& inputs, JoystickLikeType type);

        static void remove_device(i32 instance_id, std::vector<std::unique_ptr<Input>>& inputs, JoystickLikeType type);
    };


    //TODO(Totto):  differntiate different controllers and modes, e.g the switch can have pro controller, the included ones, each of them seperate etc.

    template<typename T>
    using MappingType = std::unordered_map<std::string, T>;

#if defined(__CONSOLE__)

    namespace console {
        using SettingsType = enum JOYCON;
    } // namespace console

    struct ConsoleJoystickInput : JoystickInput {
    private:
        MappingType<console::SettingsType> m_key_mappings;

    public:
        ConsoleJoystickInput(
                SDL_Joystick* joystick,
                SDL_JoystickID instance_id,
                const std::string& name,
                const MappingType<console::SettingsType>& key_mappings
        );
        [[nodiscard]] const MappingType<console::SettingsType>& key_mappings() const;

        [[nodiscard]] virtual std::string key_to_string(console::SettingsType key) const = 0;

        [[nodiscard]] virtual JoystickSettings to_normal_settings(
                const AbstractJoystickSettings<console::SettingsType>& settings
        ) const = 0;

        [[nodiscard]] JoystickSettings default_settings() const override;

        [[nodiscard]] virtual AbstractJoystickSettings<console::SettingsType> default_settings_raw() const = 0;

        [[nodiscard]] std::optional<input::NavigationEvent> handle_axis_navigation_event(const SDL_Event& event) const;
    };

#if defined(__SWITCH__)
    struct SwitchJoystickInput_Type1 : ConsoleJoystickInput {
        static constexpr sdl::GUID guid{
            sdl::GUID::ArrayType{ 0x00, 0x00, 0x38, 0xf8, 0x53, 0x77, 0x69, 0x74, 0x63, 0x68, 0x20, 0x43, 0x6f, 0x6e,
                                 0x74, 0x00 }
        };
        SwitchJoystickInput_Type1(SDL_Joystick* joystick, SDL_JoystickID instance_id, const std::string& name);

        [[nodiscard]] std::optional<NavigationEvent> get_navigation_event(const SDL_Event& event) const override;

        [[nodiscard]] std::string describe_navigation_event(NavigationEvent event) const override;

        [[nodiscard]] std::string key_to_string(console::SettingsType key) const override;

        [[nodiscard]] JoystickSettings to_normal_settings(
                const AbstractJoystickSettings<console::SettingsType>& settings
        ) const override;

        [[nodiscard]] AbstractJoystickSettings<console::SettingsType> default_settings_raw() const override;
    };


#elif defined(__3DS__)

    struct _3DSJoystickInput_Type1 : ConsoleJoystickInput {

        static constexpr sdl::GUID guid{
            sdl::GUID::ArrayType{ 0x00, 0x00, 0x10, 0x32, 0x4e, 0x69, 0x6e, 0x74, 0x65, 0x6e, 0x64, 0x6f, 0x20, 0x33,
                                 0x44, 0x00 }
        };
        _3DSJoystickInput_Type1(SDL_Joystick* joystick, SDL_JoystickID instance_id, const std::string& name);

        [[nodiscard]] std::optional<NavigationEvent> get_navigation_event(const SDL_Event& event) const override;

        [[nodiscard]] std::string describe_navigation_event(NavigationEvent event) const override;

        [[nodiscard]] std::string key_to_string(console::SettingsType key) const override;

        [[nodiscard]] JoystickSettings to_normal_settings(
                const AbstractJoystickSettings<console::SettingsType>& settings
        ) const override;

        [[nodiscard]] AbstractJoystickSettings<console::SettingsType> default_settings_raw() const override;
    };


#endif

#endif


#define X_LIST_OF_SETTINGS_KEYS \
    X_LIST_MACRO(rotate_left)   \
    X_LIST_MACRO(rotate_right)  \
    X_LIST_MACRO(move_left)     \
    X_LIST_MACRO(move_right)    \
    X_LIST_MACRO(move_down)     \
    X_LIST_MACRO(drop)          \
    X_LIST_MACRO(hold)          \
    X_LIST_MACRO(pause)         \
    X_LIST_MACRO(open_settings)


#define TRY_CONVERT(original, target, map, key) /*NOLINT(cppcoreguidelines-macro-usage)*/                       \
    do /*NOLINT(cppcoreguidelines-avoid-do-while)*/ {                                                           \
        if ((map).contains((original).key)) {                                                                   \
            (target).key = (map).at((original).key);                                                            \
        } else {                                                                                                \
            return helper::unexpected<std::string>{                                                             \
                fmt::format("While parsing key '{}': '{}' is not a valid joystick input", #key, (original).key) \
            };                                                                                                  \
        }                                                                                                       \
    } while (false)


#define SETTINGS_TO_STRING(original, target, fn, key) /*NOLINT(cppcoreguidelines-macro-usage)*/ \
    do /*NOLINT(cppcoreguidelines-avoid-do-while)*/ {                                           \
        (target).key = fn((original).key);                                                      \
    } while (false)


    struct JoystickLikeGameInput : public GameInput, public EventListener {
    private:
        std::vector<SDL_Event> m_event_buffer;
        EventDispatcher* m_event_dispatcher;

    public:
        OOPETRIS_GRAPHICS_EXPORTED explicit JoystickLikeGameInput(
                EventDispatcher* event_dispatcher,
                JoystickLikeType type
        );

        OOPETRIS_GRAPHICS_EXPORTED ~JoystickLikeGameInput() override;

        OOPETRIS_GRAPHICS_EXPORTED JoystickLikeGameInput(const JoystickLikeGameInput& input) = delete;
        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] JoystickLikeGameInput& operator=(const JoystickLikeGameInput& input
        ) = delete;

        OOPETRIS_GRAPHICS_EXPORTED JoystickLikeGameInput(JoystickLikeGameInput&& input) noexcept;
        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] JoystickLikeGameInput& operator=(JoystickLikeGameInput&& input
        ) noexcept;

        OOPETRIS_GRAPHICS_EXPORTED void handle_event(const SDL_Event& event) override;

        OOPETRIS_GRAPHICS_EXPORTED void update(SimulationStep simulation_step_index) override;

    protected:
        [[nodiscard]] virtual std::optional<InputEvent> sdl_event_to_input_event(const SDL_Event& event) const = 0;
    };


    struct JoystickGameInput : public JoystickLikeGameInput {
    private:
        JoystickInput* m_underlying_input;

    public:
        OOPETRIS_GRAPHICS_EXPORTED
        JoystickGameInput(EventDispatcher* event_dispatcher, JoystickInput* underlying_input);


        OOPETRIS_GRAPHICS_EXPORTED
        [[nodiscard]] static helper::expected<std::shared_ptr<input::JoystickGameInput>, std::string>
        get_game_input_by_settings(
                const input::InputManager& input_manager,
                EventDispatcher* event_dispatcher,
                const JoystickSettings& settings
        );

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] const JoystickInput* underlying_input() const override;

    protected:
        template<typename T>
        [[nodiscard]] static helper::expected<AbstractJoystickSettings<T>, std::string>
        try_resolve_settings(const JoystickSettings& settings, const MappingType<T>& map) {


            AbstractJoystickSettings<T> result{};


#define X_LIST_MACRO(x) TRY_CONVERT(settings, result, map, x); //NOLINT(cppcoreguidelines-macro-usage)

            X_LIST_OF_SETTINGS_KEYS

#undef X_LIST_MACRO

            return result;
        }
    };


#if defined(__CONSOLE__)
    struct ConsoleJoystickGameInput : public JoystickGameInput {
    private:
        AbstractJoystickSettings<console::SettingsType> m_settings;
        ConsoleJoystickInput* m_underlying_input;

    public:
        ConsoleJoystickGameInput(
                JoystickSettings settings,
                EventDispatcher* event_dispatcher,
                JoystickInput* underlying_input
        );

        virtual ~ConsoleJoystickGameInput();

        [[nodiscard]] std::optional<MenuEvent> get_menu_event(const SDL_Event& event) const override;

        [[nodiscard]] std::string describe_menu_event(MenuEvent event) const override;

    protected:
        [[nodiscard]] std::optional<InputEvent> sdl_event_to_input_event(const SDL_Event& event) const override;
    };
#if !defined(__SWITCH__) && !defined(__3DS__)
#error "unsupported console"
#endif
#endif
} // namespace input


namespace json_helper {


    OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] std::string
    get_key_from_object(const nlohmann::json& obj, const std::string& name);

} // namespace json_helper


namespace nlohmann {

    template<>
    struct adl_serializer<input::JoystickIdentification> {
        static input::JoystickIdentification from_json(const json& obj) {

            ::json::check_for_no_additional_keys(obj, { "guid", "name" });

            auto context = obj.at("guid");

            std::string input;
            context.get_to(input);


            const auto& value = sdl::GUID::from_string(input);

            if (not value.has_value()) {
                throw nlohmann::json::type_error::create(
                        302, fmt::format("Expected a valid GUID but got '{}': {}", input, value.error()), &context
                );
            }

            std::string name{};

            if (obj.contains("name")) {
                obj.at("name").get_to(name);
            }

            return input::JoystickIdentification{ .guid = value.value(), .name = name };
        }

        static void to_json(json& obj, const input::JoystickIdentification& identification) {
            obj = nlohmann::json::object({
                    { "guid", identification.guid.to_string() },
                    { "name",             identification.name }
            });
        }
    };

    template<>
    struct adl_serializer<input::JoystickSettings> {
        static input::JoystickSettings from_json(const json& obj) {

            ::json::check_for_no_additional_keys(
                    obj, { "type", "identification", "rotate_left", "rotate_right", "move_left", "move_right",
                           "move_down", "drop", "hold", "menu" }
            );

            const input::JoystickIdentification identification =
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

            auto identification = nlohmann::json::object();
            adl_serializer<input::JoystickIdentification>::to_json(identification, settings.identification);

            obj = nlohmann::json::object({
                    { "identification",                                                                                       identification },
                    {    "rotate_left",                                                                                 settings.rotate_left },
                    {   "rotate_right",                                                                                settings.rotate_right },
                    {      "move_left",                                                                                   settings.move_left },
                    {     "move_right",                                                                                  settings.move_right },
                    {      "move_down",                                                                                   settings.move_down },
                    {           "drop",                                                                                        settings.drop },
                    {           "hold",                                                                                        settings.hold },
                    {           "menu", nlohmann::json::object({ { "pause", settings.pause },
 { "open_settings", settings.open_settings } })                                                      }
            });
        }
    };
} // namespace nlohmann
