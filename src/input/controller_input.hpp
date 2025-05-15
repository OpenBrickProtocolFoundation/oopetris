

#pragma once

#include "helper/export_symbols.hpp"
#include "input.hpp"
#include "joystick_input.hpp"
#include "manager/sdl_controller_key.hpp"

namespace input {

    struct ControllerInput : JoystickLikeInput {
    private:
        SDL_GameController* m_controller;

    public:
        OOPETRIS_GRAPHICS_EXPORTED
        ControllerInput(SDL_GameController* m_controller, SDL_JoystickID instance_id, const std::string& name);

        OOPETRIS_GRAPHICS_EXPORTED ~ControllerInput() override;

        OOPETRIS_GRAPHICS_EXPORTED ControllerInput(const ControllerInput& input) noexcept;
        OOPETRIS_GRAPHICS_EXPORTED ControllerInput& operator=(const ControllerInput& input) noexcept;

        OOPETRIS_GRAPHICS_EXPORTED ControllerInput(ControllerInput&& input) noexcept;
        OOPETRIS_GRAPHICS_EXPORTED ControllerInput& operator=(ControllerInput&& input) noexcept;

        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED static helper::expected<std::unique_ptr<ControllerInput>, std::string>
        get_by_device_index(int device_index);

        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED std::optional<NavigationEvent> get_navigation_event(
                const SDL_Event& event
        ) const override;

        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED std::string describe_navigation_event(NavigationEvent event
        ) const override;

    private:
        [[nodiscard]] std::optional<input::NavigationEvent> handle_axis_navigation_event(const SDL_Event& event) const;
    };

    struct ControllerSettings {
        sdl::ControllerKey rotate_left;
        sdl::ControllerKey rotate_right;
        sdl::ControllerKey move_left;
        sdl::ControllerKey move_right;
        sdl::ControllerKey move_down;
        sdl::ControllerKey drop;
        sdl::ControllerKey hold;

        sdl::ControllerKey pause;
        sdl::ControllerKey open_settings;


        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED helper::expected<void, std::string> validate() const;


        [[nodiscard]] static ControllerSettings default_settings() {
            return ControllerSettings{ .rotate_left = sdl::ControllerKey{ SDL_CONTROLLER_BUTTON_LEFTSHOULDER },
                                       .rotate_right = sdl::ControllerKey{ SDL_CONTROLLER_BUTTON_RIGHTSHOULDER },
                                       .move_left = sdl::ControllerKey{ SDL_CONTROLLER_BUTTON_DPAD_LEFT },
                                       .move_right = sdl::ControllerKey{ SDL_CONTROLLER_BUTTON_DPAD_RIGHT },
                                       .move_down = sdl::ControllerKey{ SDL_CONTROLLER_BUTTON_DPAD_DOWN },
                                       .drop = sdl::ControllerKey{ SDL_CONTROLLER_BUTTON_A },
                                       .hold = sdl::ControllerKey{ SDL_CONTROLLER_BUTTON_B },
                                       .pause = sdl::ControllerKey{ SDL_CONTROLLER_BUTTON_START },
                                       .open_settings = sdl::ControllerKey{ SDL_CONTROLLER_BUTTON_GUIDE } };
        }
    };


    struct ControllerGameInput : public JoystickLikeGameInput {
    private:
        ControllerSettings m_settings;
        ControllerInput* m_underlying_input;


    public:
        OOPETRIS_GRAPHICS_EXPORTED ControllerGameInput(
                ControllerSettings settings,
                EventDispatcher* event_dispatcher,
                ControllerInput* underlying_input
        );

        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED std::optional<MenuEvent> get_menu_event(const SDL_Event& event
        ) const override;

        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED std::string describe_menu_event(MenuEvent event) const override;

        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED const ControllerInput* underlying_input() const override;

    protected:
        [[nodiscard]] std::optional<InputEvent> sdl_event_to_input_event(const SDL_Event& event) const override;
    };


} // namespace input


namespace json_helper {


    [[nodiscard]] sdl::ControllerKey get_controller_key(const nlohmann::json& obj, const std::string& name);

} // namespace json_helper


namespace nlohmann {
    template<>
    struct adl_serializer<input::ControllerSettings> {
        static input::ControllerSettings from_json(const json& obj) {

            ::json::check_for_no_additional_keys(
                    obj, { "type", "rotate_left", "rotate_right", "move_left", "move_right", "move_down", "drop",
                           "hold", "menu" }
            );

            const auto rotate_left = json_helper::get_controller_key(obj, "rotate_left");
            const auto rotate_right = json_helper::get_controller_key(obj, "rotate_right");
            const auto move_left = json_helper::get_controller_key(obj, "move_left");
            const auto move_right = json_helper::get_controller_key(obj, "move_right");
            const auto move_down = json_helper::get_controller_key(obj, "move_down");
            const auto drop = json_helper::get_controller_key(obj, "drop");
            const auto hold = json_helper::get_controller_key(obj, "hold");

            const auto& menu = obj.at("menu");

            ::json::check_for_no_additional_keys(menu, { "pause", "open_settings" });

            const auto pause = json_helper::get_controller_key(menu, "pause");
            const auto open_settings = json_helper::get_controller_key(menu, "open_settings");

            auto settings = input::ControllerSettings{ .rotate_left = rotate_left,
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

        static void to_json(json& obj, const input::ControllerSettings& settings) {
            obj = nlohmann::json::object({
                    { "rotate_left", settings.rotate_left.to_string() },
                    { "rotate_right", settings.rotate_right.to_string() },
                    { "move_left", settings.move_left.to_string() },
                    { "move_right", settings.move_right.to_string() },
                    { "move_down", settings.move_down.to_string() },
                    { "drop", settings.drop.to_string() },
                    { "hold", settings.hold.to_string() },
                    {
                     "menu", nlohmann::json::object({
                                    { "pause", settings.pause.to_string() },
                                    { "open_settings", settings.open_settings.to_string() },
                            }),
                     }
            });
        }
    };
} // namespace nlohmann
