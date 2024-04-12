#pragma once

#include "helper/expected.hpp"
#include "helper/types.hpp"
#include "helper/utils.hpp"

#include <algorithm>
#include <cmath>
#include <fmt/format.h>
#include <ostream>
#include <string>

struct Color;

namespace color {

    enum class SerializeMode : u8 { Hex, RGB, HSV };
}

struct HSVColor {
    double h;
    double s;
    double v;
    u8 a;

    constexpr HSVColor(double h, double s, double v, u8 a) // NOLINT(bugprone-easily-swappable-parameters)
        : h{ h },
          s{ s },
          v{ v },
          a{ a } {

        if (utils::is_constant_evaluated()) {

            CONSTEVAL_STATIC_ASSERT(h >= 0.0 && h <= 360.0, "h has to be in range 0.0 - 360.0");
            CONSTEVAL_STATIC_ASSERT(s >= 0.0 && s <= 1.0, "s has to be in range 0.0 - 1.0");
            CONSTEVAL_STATIC_ASSERT(v >= 0.0 && v <= 1.0, "v has to be in range 0.0 - 1.0");

        } else {

            if (h < 0.0 || h > 360.0) {
                throw std::runtime_error{ fmt::format("h has to be in range 0.0 - 360.0 but was: {}", h) };
            }
            if (s < 0.0 || s > 1.0) {

                throw std::runtime_error{ fmt::format("s has to be in range 0.0 - 1.0 but was: {}", s) };
            }
            if (v < 0.0 || v > 1.0) {
                throw std::runtime_error{ fmt::format("v has to be in range 0.0 - 1.0 but was: {}", v) };
            }
        }
    }

    constexpr HSVColor() : HSVColor{ 0.0, 0.0, 0.0, 0 } { }

    constexpr HSVColor(double h, double s, double v) : HSVColor{ h, s, v, 0xFF } { }

    [[nodiscard]] static helper::expected<HSVColor, std::string> from_string(const std::string& value);

    [[nodiscard]] static helper::expected<std::tuple<HSVColor, color::SerializeMode, bool>, std::string>
    from_string_with_info(const std::string& value);

    [[nodiscard]] Color to_rgb_color() const;

    [[nodiscard]] std::string to_string(bool force_alpha = false) const;

    std::ostream& operator<<(std::ostream& os) const;
};

namespace {
    //TODO: if everything (also libc++ ) supports c++23 , the std functions are constexpr, so we can use them
    template<typename T>
    constexpr T fmod_constexpr(T value, T divisor) {
        if (not utils::is_constant_evaluated()) {
            return std::fmod(value, divisor);
        }

        return value - static_cast<T>(static_cast<u64>(value / divisor)) * divisor;
    }

    template<typename T>
    constexpr T fabs_constexpr(T value) {
        if (not utils::is_constant_evaluated()) {
            return std::fabs(value);
        }

        if (value == static_cast<T>(-0.0)) {
            return static_cast<T>(0.0);
        }

        if (value < static_cast<T>(0.0)) {
            return -value;
        }

        return value;
    }

    template<typename T>
    constexpr T round_constexpr(T value) {
        if (not utils::is_constant_evaluated()) {
            return std::round(value);
        }

        T truncated = static_cast<T>(static_cast<i64>(value));

        if (value - truncated >= 0.5) {
            truncated += static_cast<T>(1.0);
        }

        return truncated;
    }
} // namespace

struct Color {
    u8 r;
    u8 g;
    u8 b;
    u8 a;


    constexpr Color(u8 r, u8 g, u8 b, u8 a) // NOLINT(bugprone-easily-swappable-parameters)
        : r{ r },
          g{ g },
          b{ b },
          a{ a } { }

    constexpr Color() : Color{ 0, 0, 0, 0 } { }

    constexpr Color(u8 r, u8 g, u8 b) : Color{ r, g, b, 0xFF } { }

    [[nodiscard]] static helper::expected<Color, std::string> from_string(const std::string& value);

    [[nodiscard]] static helper::expected<std::tuple<Color, color::SerializeMode, bool>, std::string>
    from_string_with_info(const std::string& value);


    [[nodiscard]] HSVColor to_hsv_color() const;

    constexpr Color(const HSVColor& color) {

        using FloatType = double; //for more precision use "long double" here

        // taken from https://scratch.mit.edu/discuss/topic/694772/

        const auto h = static_cast<FloatType>(color.h);
        const auto s = static_cast<FloatType>(color.s);
        const auto v = static_cast<FloatType>(color.v);

        const FloatType chroma = v * s;

        FloatType hue = h;
        if (hue >= static_cast<FloatType>(360.0)) {
            hue = static_cast<FloatType>(0.0);
        }


        const FloatType x = chroma
                            * (static_cast<FloatType>(1.0)
                               - fabs_constexpr(
                                       fmod_constexpr(hue / static_cast<FloatType>(60.0), static_cast<FloatType>(2.0))
                                       - static_cast<FloatType>(1.0)
                               ));

        const u64 index = static_cast<u64>(hue / static_cast<FloatType>(60.0));

        FloatType d_r{};
        FloatType d_g{};
        FloatType d_b{};

        switch (index) {
            case 0:
                d_r = chroma;
                d_g = x;
                d_b = static_cast<FloatType>(0.0);
                break;
            case 1:
                d_r = x;
                d_g = chroma;
                d_b = static_cast<FloatType>(0.0);
                break;
            case 2:
                d_r = static_cast<FloatType>(0.0);
                d_g = chroma;
                d_b = x;
                break;
            case 3:
                d_r = static_cast<FloatType>(0.0);
                d_g = x;
                d_b = chroma;
                break;
            case 4:
                d_r = x;
                d_g = static_cast<FloatType>(0.0);
                d_b = chroma;
                break;
            case 5:
                d_r = chroma;
                d_g = static_cast<FloatType>(0.0);
                d_b = x;
                break;
            default:
                utils::unreachable();
        }


        const FloatType m = v - chroma;

        const auto finish_value = [m](FloatType value) -> u8 {
            const auto result =
                    std::clamp<FloatType>(value + m, static_cast<FloatType>(0.0), static_cast<FloatType>(1.0))
                    * static_cast<FloatType>(0xFF);

            return static_cast<u8>(round_constexpr(result));
        };

        this->r = finish_value(d_r);
        this->g = finish_value(d_g);
        this->b = finish_value(d_b);
        this->a = color.a;
    }

    [[nodiscard]] constexpr bool operator==(const Color& other) const {
        return std::tuple<u8, u8, u8, u8>{ r, g, b, a }
               == std::tuple<u8, u8, u8, u8>{ other.r, other.g, other.b, other.a };
    }


    [[nodiscard]] static constexpr Color red(u8 alpha = 0xFF) {
        return Color{ 0xFF, 0, 0, alpha };
    };

    [[nodiscard]] static constexpr Color green(u8 alpha = 0xFF) {
        return Color{ 0, 0xFF, 0, alpha };
    };

    [[nodiscard]] static constexpr Color blue(u8 alpha = 0xFF) {
        return Color{ 0, 0, 0xFF, alpha };
    };

    [[nodiscard]] static constexpr Color black(u8 alpha = 0xFF) {
        return Color{ 0, 0, 0, alpha };
    };

    [[nodiscard]] static constexpr Color white(u8 alpha = 0xFF) {
        return Color{ 0xFF, 0xFF, 0xFF, alpha };
    };

    [[nodiscard]] std::string to_string(color::SerializeMode mode = color::SerializeMode::RGB, bool force_alpha = false)
            const;

    std::ostream& operator<<(std::ostream& os) const;
};
