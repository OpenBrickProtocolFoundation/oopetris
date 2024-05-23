

#pragma once

#include "input.hpp"
#include "joystick_input.hpp"
#include "manager/sdl_controller_key.hpp"

namespace input {

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


        [[nodiscard]] helper::expected<bool, std::string> validate() const;


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


    //TODO(Totto): use these additional mappings!
    // see: https://github.com/mdqinc/SDL_GameControllerDB?tab=readme-ov-file


    struct ControllerInput : JoystickLikeInput {
    private:
        SDL_GameController* m_controller;

    public:
        ControllerInput(SDL_GameController* m_controller, SDL_JoystickID instance_id, const std::string& name);

        ~ControllerInput() override;

        ControllerInput(const ControllerInput& input) noexcept;
        ControllerInput& operator=(const ControllerInput& input) noexcept;

        ControllerInput(ControllerInput&& input) noexcept;
        ControllerInput& operator=(ControllerInput&& input) noexcept;

        [[nodiscard]] static helper::expected<std::unique_ptr<ControllerInput>, std::string> get_by_device_index(
                int device_index
        );

        [[nodiscard]] helper::optional<NavigationEvent> get_navigation_event(const SDL_Event& event) const override;

        [[nodiscard]] std::string describe_navigation_event(NavigationEvent event) const override;

    private:
        [[nodiscard]] helper::optional<input::NavigationEvent> handle_axis_navigation_event(const SDL_Event& event
        ) const;

        // Add get_game_input method!
    };

} // namespace input
