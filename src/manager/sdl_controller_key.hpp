#pragma once

#include <core/helper/expected.hpp>


#include <SDL.h>
#include <fmt/format.h>
#include <string>


namespace sdl {

    struct ControllerKey {
    private:
        SDL_GameControllerButton m_button;

    public:
        explicit ControllerKey(SDL_GameControllerButton button);

        static helper::expected<ControllerKey, std::string> from_string(const std::string& value);

        [[nodiscard]] bool operator==(const ControllerKey& other) const;

        [[nodiscard]] std::string to_string() const;
    };


} // namespace sdl

template<>
struct fmt::formatter<sdl::ControllerKey> : fmt::formatter<std::string> {
    auto format(const sdl::ControllerKey& key, format_context& ctx) const {
        return formatter<std::string>::format(key.to_string(), ctx);
    }
};
