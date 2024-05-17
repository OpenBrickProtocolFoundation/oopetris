#include "keyboard_input.hpp"
#include "helper/optional.hpp"
#include "helper/utils.hpp"
#include "input/game_input.hpp"
#include "input/input.hpp"


input::KeyboardInput::KeyboardInput() : input::Input{ "keyboard", InputType::Keyboard } { }


[[nodiscard]] helper::optional<input::NavigationEvent> input::KeyboardInput::get_navigation_event(const SDL_Event& event
) const {


    if (event.type == SDL_KEYDOWN) {

        const auto key = SDL::Key{ event.key.keysym };

        if (key == SDL::Key{ SDLK_RETURN } or key == SDL::Key{ SDLK_SPACE }) {
            return NavigationEvent::OK;
        }

        if (key == SDL::Key{ SDLK_DOWN } or key == SDL::Key{ SDLK_s }) {
            return NavigationEvent::DOWN;
        }


        if (key == SDL::Key{ SDLK_UP } or key == SDL::Key{ SDLK_w }) {
            return NavigationEvent::UP;
        }


        if (key == SDL::Key{ SDLK_LEFT } or key == SDL::Key{ SDLK_a }) {
            return NavigationEvent::LEFT;
        }


        if (key == SDL::Key{ SDLK_RIGHT } or key == SDL::Key{ SDLK_d }) {
            return NavigationEvent::RIGHT;
        }


        if (key == SDL::Key{ SDLK_ESCAPE } or key == SDL::Key{ SDLK_BACKSPACE }) {
            return NavigationEvent::BACK;
        }


        if (key == SDL::Key{ SDLK_TAB }) {
            return NavigationEvent::TAB;
        }
    }

    return helper::nullopt;
}


[[nodiscard]] std::string input::KeyboardInput::describe_navigation_event(NavigationEvent event) const {


    switch (event) {

        case NavigationEvent::OK:
            return fmt::format("{} or {}", SDL::Key{ SDLK_RETURN }, SDL::Key{ SDLK_SPACE });
        case NavigationEvent::DOWN:
            return fmt::format("{} or {}", SDL::Key{ SDLK_DOWN }, SDL::Key{ SDLK_s });
        case NavigationEvent::UP:
            return fmt::format("{} or {}", SDL::Key{ SDLK_UP }, SDL::Key{ SDLK_w });
        case NavigationEvent::LEFT:
            return fmt::format("{} or {}", SDL::Key{ SDLK_LEFT }, SDL::Key{ SDLK_a });
        case NavigationEvent::RIGHT:
            return fmt::format("{} or {}", SDL::Key{ SDLK_RIGHT }, SDL::Key{ SDLK_d });
        case NavigationEvent::BACK:
            return fmt::format("{} or {}", SDL::Key{ SDLK_ESCAPE }, SDL::Key{ SDLK_BACKSPACE });
        case NavigationEvent::TAB:
            return fmt::format("{}", SDL::Key{ SDLK_TAB });
        default:
            utils::unreachable();
    }
}


void input::KeyboardGameInput::handle_event(const SDL_Event& event) {
    m_event_buffer.push_back(event);
}

void input::KeyboardGameInput::update(SimulationStep simulation_step_index) {
    for (const auto& event : m_event_buffer) {
        const auto input_event = sdl_event_to_input_event(event);
        if (input_event.has_value()) {
            GameInput::handle_event(*input_event, simulation_step_index);
        }
    }
    m_event_buffer.clear();

    GameInput::update(simulation_step_index);
}

helper::optional<InputEvent> input::KeyboardGameInput::sdl_event_to_input_event(const SDL_Event& event) const {
    if (event.type == SDL_KEYDOWN and event.key.repeat == 0) {
        const auto key = SDL::Key{ event.key.keysym };
        if (key == m_settings.rotate_left) {
            return InputEvent::RotateLeftPressed;
        }
        if (key == m_settings.rotate_right) {
            return InputEvent::RotateRightPressed;
        }
        if (key == m_settings.move_down) {
            return InputEvent::MoveDownPressed;
        }
        if (key == m_settings.move_left) {
            return InputEvent::MoveLeftPressed;
        }
        if (key == m_settings.move_right) {
            return InputEvent::MoveRightPressed;
        }
        if (key == m_settings.drop) {
            return InputEvent::DropPressed;
        }
        if (key == m_settings.hold) {
            return InputEvent::HoldPressed;
        }
    } else if (event.type == SDL_KEYUP) {
        const auto key = SDL::Key{ event.key.keysym };
        if (key == m_settings.rotate_left) {
            return InputEvent::RotateLeftReleased;
        }
        if (key == m_settings.rotate_right) {
            return InputEvent::RotateRightReleased;
        }
        if (key == m_settings.move_down) {
            return InputEvent::MoveDownReleased;
        }
        if (key == m_settings.move_left) {
            return InputEvent::MoveLeftReleased;
        }
        if (key == m_settings.move_right) {
            return InputEvent::MoveRightReleased;
        }
        if (key == m_settings.drop) {
            return InputEvent::DropReleased;
        }
        if (key == m_settings.hold) {
            return InputEvent::HoldReleased;
        }
    }
    return helper::nullopt;
}

input::KeyboardGameInput::KeyboardGameInput(const KeyboardSettings& settings, EventDispatcher* event_dispatcher)
    : GameInput{ GameInputType::Keyboard },
      m_settings{ settings },
      m_event_dispatcher{ event_dispatcher } {
    m_event_dispatcher->register_listener(this);
}

input::KeyboardGameInput::~KeyboardGameInput() {
    m_event_dispatcher->unregister_listener(this);
}


[[nodiscard]] helper::expected<bool, std::string> input::KeyboardSettings::validate() const {

    const std::vector<SDL::Key> to_use{ rotate_left, rotate_right, move_left, move_right,   move_down,
                                        drop,        hold,         pause,     open_settings };

    return input::InputSettings::has_unique_members(to_use);
}

SDL::Key json_helper::get_key(const nlohmann::json& j, const std::string& name) {

    auto context = j.at(name);

    std::string input;
    context.get_to(input);

    const auto& value = SDL::Key::from_string(input);

    if (not value.has_value()) {
        throw nlohmann::json::type_error::create(
                302, fmt::format("Expected a valid Key for key '{}', but got '{}': {}", name, input, value.error()),
                &context
        );
    }
    return value.value();
}


[[nodiscard]] helper::optional<input::MenuEvent> input::KeyboardGameInput::get_menu_event(const SDL_Event& event
) const {

    if (event.type == SDL_KEYDOWN and event.key.repeat == 0) {
        const auto key = SDL::Key{ event.key.keysym };
        if (key == m_settings.pause) {
            return MenuEvent::Pause;
        }
        if (key == m_settings.open_settings) {
            return MenuEvent::OpenSettings;
        }
    }

    return helper::nullopt;
}

[[nodiscard]] std::string input::KeyboardGameInput::describe_menu_event(MenuEvent event) const {
    switch (event) {
        case input::MenuEvent::Pause:
            return m_settings.pause.to_string();
        case input::MenuEvent::OpenSettings:
            return m_settings.open_settings.to_string();
        default:
            utils::unreachable();
    }
}
