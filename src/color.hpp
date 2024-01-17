#pragma once

#include "types.hpp"

#include <SDL.h>

struct Color {
    u8 r;
    u8 g;
    u8 b;
    u8 a;

    constexpr Color() : Color{ 0, 0, 0, 0 } { }

    constexpr Color(u8 r, u8 g, u8 b, u8 a) : r{ r }, g{ g }, b{ b }, a{ a } { }

    constexpr Color(u8 r, u8 g, u8 b) : Color{ r, g, b, 0xFF } { }

    Uint32 to_sdl(const SDL_Surface* surface) const;

    static constexpr Color red(u8 alpha = 0xFF) {
        return Color{ 0xFF, 0, 0, alpha };
    };

    static constexpr Color green(u8 alpha = 0xFF) {
        return Color{ 0, 0xFF, 0, alpha };
    };

    static constexpr Color blue(u8 alpha = 0xFF) {
        return Color{ 0, 0, 0xFF, alpha };
    };

    static constexpr Color black(u8 alpha = 0xFF) {
        return Color{ 0, 0, 0, alpha };
    };

    static constexpr Color white(u8 alpha = 0xFF) {
        return Color{ 0xFF, 0xFF, 0xFF, alpha };
    };
};
