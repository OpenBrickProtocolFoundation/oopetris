#pragma once

#include "helper/types.hpp"

#if !defined(_NO_SDL)
#include <SDL.h>
#endif

#include <iomanip>
#include <ostream>

struct Color {
    u8 r;
    u8 g;
    u8 b;
    u8 a;

    constexpr Color() : Color{ 0, 0, 0, 0 } { }

    constexpr Color(u8 r, u8 g, u8 b, u8 a) // NOLINT(bugprone-easily-swappable-parameters)
        : r{ r },
          g{ g },
          b{ b },
          a{ a } { }

    constexpr Color(u8 r, u8 g, u8 b) : Color{ r, g, b, 0xFF } { }

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

#if !defined(_NO_SDL)
    [[nodiscard]] SDL_Color to_sdl_color() const {
        return SDL_Color{ r, g, b, a };
    }
#endif


    // helper to display values
    constexpr std::ostream& operator<<(std::ostream& os) const {
        std::ostringstream oss;
        oss << std::hex << std::setfill('0') << '#' << std::setw(2) << static_cast<int>(r) << std::setw(2)
            << static_cast<int>(g) << std::setw(2) << static_cast<int>(b) << std::setw(2) << static_cast<int>(a) << ';';
        return os << oss.str();
    }
};


// taken from https://stackoverflow.com/questions/66813961/c-constexpr-constructor-for-colours:
// heavily modified in many places
namespace {
// define a consteval assert, it isn't a pretty error message, but there's nothing we can do against that atm :(
// this https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2024/p2758r2.html tries to fix it
#define CONSTEVAL_STATIC_ASSERT(CHECK, MSG)                                                                                                     \
    ((CHECK) ? void(0) : [] {                                                                                                                   \
        /* If you see this really bad c++ error message, follow the origin of MSG, to see the real error message, c++ error messages suck xD */ \
        throw MSG;                                                                                                                              \
    }())


    // decode a single_hex_number
    consteval u8 single_hex_number(char n) {
        if (n >= '0' && n <= '9') {
            return n - '0';
        }

        if (n >= 'A' && n <= 'F') {
            return n - 'A' + 10;
        }

        if (n >= 'a' && n <= 'f') {
            return n - 'a' + 10;
        }

        CONSTEVAL_STATIC_ASSERT(false, "the input must be a valid hex character");
    }

    // decode a single color value
    consteval u8 single_color_value(const char* b) {
        return single_hex_number(b[0]) << 4 | single_hex_number(b[1]);
    }

    constexpr int red_offset = 1; // offsets in C strings
    constexpr int green_offset = 3;
    constexpr int blue_offset = 5;
    constexpr int alpha_offset = 7;
    constexpr int final_size = 9;
} // namespace

consteval Color operator""_rgb(const char* s, std::size_t size) {
    CONSTEVAL_STATIC_ASSERT(size == alpha_offset, "RGB literals must be of size 7");
    CONSTEVAL_STATIC_ASSERT(s[0] == '#', "RGB literals must start with '#'");
    return { single_color_value(s + red_offset), single_color_value(s + green_offset),
             single_color_value(s + blue_offset) };
}

consteval Color operator""_rgba(const char* s, std::size_t size) {
    CONSTEVAL_STATIC_ASSERT(size == final_size, "RGBA literals must be of size 9");
    CONSTEVAL_STATIC_ASSERT(s[0] == '#', "RGBA literals must start with '#'");
    return { single_color_value(s + red_offset), single_color_value(s + green_offset),
             single_color_value(s + blue_offset), single_color_value(s + alpha_offset) };
}
