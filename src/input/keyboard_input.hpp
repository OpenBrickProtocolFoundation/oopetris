#pragma once

#include "SDL_keycode.h"
#include "game_input.hpp"
#include "helper/expected.hpp"
#include "helper/parse_json.hpp"
#include "input.hpp"
#include "manager/event_dispatcher.hpp"
#include "manager/sdl_key.hpp"


#include <stdexcept>


namespace input {


    struct KeyboardInput : Input {

    public:
        KeyboardInput();
        virtual ~KeyboardInput();


        [[nodiscard]] helper::optional<NavigationEvent> get_navigation_event(const SDL_Event& event) const override;

        [[nodiscard]] std::string describe_navigation_event(NavigationEvent event) const override;
    };


    //TODO: don't default initialize all settings, but rather provide a static default setting, so that everything has to be set explicitly, or you can use the default explicitly
    struct KeyboardSettings : InputSettings {
        SDL::Key rotate_left = SDL::Key{ SDLK_LEFT };
        SDL::Key rotate_right = SDL::Key{ SDLK_RIGHT };
        SDL::Key move_left = SDL::Key{ SDLK_a };
        SDL::Key move_right = SDL::Key{ SDLK_d };
        SDL::Key move_down = SDL::Key{ SDLK_a };
        SDL::Key drop = SDL::Key{ SDLK_s };
        SDL::Key hold = SDL::Key{ SDLK_TAB };

        SDL::Key pause = SDL::Key{ SDLK_ESCAPE };
        SDL::Key open_settings = SDL::Key{ SDLK_e };


        [[nodiscard]] helper::expected<bool, std::string> validate() const override;
    };


    struct KeyboardGameInput : public GameInput, public EventListener {
    private:
        KeyboardSettings m_settings;
        std::vector<SDL_Event> m_event_buffer;
        EventDispatcher* m_event_dispatcher;

    public:
        KeyboardGameInput(const KeyboardSettings& settings, EventDispatcher* event_dispatcher);

        ~KeyboardGameInput() override;

        void handle_event(const SDL_Event& event) override;

        void update(SimulationStep simulation_step_index) override;

        [[nodiscard]] helper::optional<MenuEvent> get_menu_event(const SDL_Event& event) const override;

        [[nodiscard]] std::string describe_menu_event(MenuEvent event) const override;


    private:
        [[nodiscard]] helper::optional<InputEvent> sdl_event_to_input_event(const SDL_Event& event) const;
    };
} // namespace input


inline void to_json(nlohmann::json& j, const input::KeyboardSettings& settings) {
    j = nlohmann::json{
        { "rotate_left", settings.rotate_left.name() },
        { "rotate_right", settings.rotate_right.name() },
        { "move_left", settings.move_left.name() },
        { "move_right", settings.move_right.name() },
        { "move_down", settings.move_down.name() },
        { "drop", settings.drop.name() },
        { "hold", settings.hold.name() },
        {
         "menu", nlohmann::json{
                        { "pause", settings.pause.name() },
                        { "open_settings", settings.open_settings.name() },
                }, }
    };
}

namespace json_helper {


    [[nodiscard]] SDL::Key get_key(const nlohmann::json& j, const std::string& name);

} // namespace json_helper

inline void from_json(const nlohmann::json& j, input::KeyboardSettings& settings) {

    json::check_for_no_additional_keys(
            j, { "type", "rotate_left", "rotate_right", "move_left", "move_right", "move_down", "drop", "hold", "menu" }
    );

    settings.rotate_left = json_helper::get_key(j, "rotate_left");
    settings.rotate_right = json_helper::get_key(j, "rotate_right");
    settings.move_left = json_helper::get_key(j, "move_left");
    settings.move_right = json_helper::get_key(j, "move_right");
    settings.move_down = json_helper::get_key(j, "move_down");
    settings.drop = json_helper::get_key(j, "drop");
    settings.hold = json_helper::get_key(j, "hold");

    const auto& menu = j.at("menu");

    json::check_for_no_additional_keys(menu, { "pause", "open_settings" });

    settings.pause = json_helper::get_key(menu, "pause");
    settings.open_settings = json_helper::get_key(menu, "open_settings");

    const auto is_valid = settings.validate();
    if (not is_valid.has_value()) {
        throw std::runtime_error(is_valid.error());
    }
}