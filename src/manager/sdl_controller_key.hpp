#pragma once

#include <core/helper/expected.hpp>

#include "helper/windows.hpp"

#include <SDL.h>
#include <fmt/format.h>
#include <string>

namespace sdl {

    struct ControllerKey {
    private:
        SDL_GameControllerButton m_button;

    public:
        OOPETRIS_GRAPHICS_EXPORTED explicit ControllerKey(SDL_GameControllerButton button);

        OOPETRIS_GRAPHICS_EXPORTED static helper::expected<ControllerKey, std::string> from_string(
                const std::string& value
        );

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] bool operator==(const ControllerKey& other) const;

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] std::string to_string() const;
    };


} // namespace sdl

template<>
struct fmt::formatter<sdl::ControllerKey> : fmt::formatter<std::string> {
    auto format(const sdl::ControllerKey& key, format_context& ctx) const {
        return formatter<std::string>::format(key.to_string(), ctx);
    }
};
