
#include "sdl_controller_key.hpp"


sdl::ControllerKey::ControllerKey(SDL_GameControllerButton button) : m_button{ button } { }

helper::expected<sdl::ControllerKey, std::string> sdl::ControllerKey::from_string(const std::string& value) {

    const auto key = SDL_GameControllerGetButtonFromString(value.c_str());
    if (key == SDL_CONTROLLER_BUTTON_INVALID) {
        return helper::unexpected<std::string>{
            fmt::format("No SDL controller key for the name '{}': {}", value, SDL_GetError())
        };
    }

    return ControllerKey{ key };
}

[[nodiscard]] bool sdl::ControllerKey::operator==(const ControllerKey& other) const {
    return m_button == other.m_button;
}


[[nodiscard]] std::string sdl::ControllerKey::to_string() const {

    const auto* name = SDL_GameControllerGetStringForButton(m_button);
    if (name == nullptr or std::strlen(name) == 0) {
        throw std::runtime_error(fmt::format(
                "No name for the SDL key {}: {}", static_cast<std::underlying_type_t<decltype(m_button)>>(m_button),
                SDL_GetError()
        ));
    }
    return std::string{ name };
}
