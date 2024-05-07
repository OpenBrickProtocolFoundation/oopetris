#pragma once

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
    struct KeyboardSettings {
        SDL::Key rotate_left;
        SDL::Key rotate_right;
        SDL::Key move_left;
        SDL::Key move_right;
        SDL::Key move_down;
        SDL::Key drop;
        SDL::Key hold;

        SDL::Key pause;
        SDL::Key open_settings;


        [[nodiscard]] helper::expected<bool, std::string> validate() const;

        [[nodiscard]] static KeyboardSettings default_settings() {
            return KeyboardSettings{ .rotate_left = SDL::Key{ SDLK_LEFT },
                                     .rotate_right = SDL::Key{ SDLK_RIGHT },
                                     .move_left = SDL::Key{ SDLK_a },
                                     .move_right = SDL::Key{ SDLK_d },
                                     .move_down = SDL::Key{ SDLK_a },
                                     .drop = SDL::Key{ SDLK_s },
                                     .hold = SDL::Key{ SDLK_TAB },
                                     .pause = SDL::Key{ SDLK_ESCAPE },
                                     .open_settings = SDL::Key{ SDLK_e } };
        }
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


namespace json_helper {


    [[nodiscard]] SDL::Key get_key(const nlohmann::json& j, const std::string& name);

} // namespace json_helper


namespace nlohmann {
    template<>
    struct adl_serializer<input::KeyboardSettings> {
        static input::KeyboardSettings from_json(const json& j) {

            ::json::check_for_no_additional_keys(
                    j, { "type", "rotate_left", "rotate_right", "move_left", "move_right", "move_down", "drop", "hold",
                         "menu" }
            );

            const auto rotate_left = json_helper::get_key(j, "rotate_left");
            const auto rotate_right = json_helper::get_key(j, "rotate_right");
            const auto move_left = json_helper::get_key(j, "move_left");
            const auto move_right = json_helper::get_key(j, "move_right");
            const auto move_down = json_helper::get_key(j, "move_down");
            const auto drop = json_helper::get_key(j, "drop");
            const auto hold = json_helper::get_key(j, "hold");

            const auto& menu = j.at("menu");

            ::json::check_for_no_additional_keys(menu, { "pause", "open_settings" });

            const auto pause = json_helper::get_key(menu, "pause");
            const auto open_settings = json_helper::get_key(menu, "open_settings");

            auto settings = input::KeyboardSettings{ .rotate_left = rotate_left,
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

        static void to_json(json& j, const input::KeyboardSettings& settings) {
            j = nlohmann::json{
                { "rotate_left", settings.rotate_left.to_string() },
                { "rotate_right", settings.rotate_right.to_string() },
                { "move_left", settings.move_left.to_string() },
                { "move_right", settings.move_right.to_string() },
                { "move_down", settings.move_down.to_string() },
                { "drop", settings.drop.to_string() },
                { "hold", settings.hold.to_string() },
                {
                 "menu", nlohmann::json{
                                { "pause", settings.pause.to_string() },
                                { "open_settings", settings.open_settings.to_string() },
                        }, }
            };
        }
    };
} // namespace nlohmann
