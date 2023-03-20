#pragma once

#include <cstdint>

struct Color {
    std::uint8_t r;
    std::uint8_t g;
    std::uint8_t b;
    std::uint8_t a;

    constexpr Color() : Color{ 0, 0, 0, 0 } { }

    constexpr Color(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a) : r{ r }, g{ g }, b{ b }, a{ a } { }

    constexpr Color(std::uint8_t r, std::uint8_t g, std::uint8_t b) : Color{ r, g, b, 255 } { }

    static constexpr Color red(std::uint8_t alpha = 255) {
        return Color{ 255, 0, 0, alpha };
    };

    static constexpr Color green(std::uint8_t alpha = 255) {
        return Color{ 0, 255, 0, alpha };
    };

    static constexpr Color blue(std::uint8_t alpha = 255) {
        return Color{ 0, 0, 255, alpha };
    };

    static constexpr Color black(std::uint8_t alpha = 255) {
        return Color{ 0, 0, 0, alpha };
    };

    static constexpr Color white(std::uint8_t alpha = 255) {
        return Color{ 255, 255, 255, alpha };
    };
};
