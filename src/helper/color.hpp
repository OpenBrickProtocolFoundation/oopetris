#pragma once

#include "helper/expected.hpp"
#include "helper/types.hpp"
#include "helper/utils.hpp"

#include <algorithm>
#include <fmt/format.h>
#include <ostream>
#include <string>


struct Color;

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

    [[nodiscard]] Color to_rgb_color() const;

    [[nodiscard]] std::string to_string() const;

    std::ostream& operator<<(std::ostream& os) const;
};

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

    [[nodiscard]] HSVColor to_hsv_color() const;

    constexpr Color(const HSVColor& color) {

        const auto set_color = [&color, this](double r, double g, double b) {
            this->r = static_cast<u8>(std::clamp(r, 0.0, 1.0) * static_cast<double>(0xFF));
            this->g = static_cast<u8>(std::clamp(g, 0.0, 1.0) * static_cast<double>(0xFF));
            this->b = static_cast<u8>(std::clamp(b, 0.0, 1.0) * static_cast<double>(0xFF));
            this->a = color.a;
        };

        // taken from https://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb-in-range-0-255-for-both
        double hh{};
        double p{};
        double q{};
        double t{};
        double ff{};

        long i{};

        double d_r{};
        double d_g{};
        double d_b{};

        if (color.s <= 0.0) { // < is bogus, just shuts up warnings
            d_r = color.v;
            d_g = color.v;
            d_b = color.v;
            set_color(d_r, d_g, d_b);
            return;
        }

        hh = color.h;
        if (hh >= 360.0) {
            hh = 0.0;
        }

        hh /= 60.0;
        i = static_cast<long>(hh);
        ff = hh - static_cast<double>(i);

        p = color.v * (1.0 - color.s);
        q = color.v * (1.0 - (color.s * ff));
        t = color.v * (1.0 - (color.s * (1.0 - ff)));

        switch (i) {
            case 0:
                d_r = color.v;
                d_g = t;
                d_b = p;
                break;
            case 1:
                d_r = q;
                d_g = color.v;
                d_b = p;
                break;
            case 2:
                d_r = p;
                d_g = color.v;
                d_b = t;
                break;
            case 3:
                d_r = p;
                d_g = q;
                d_b = color.v;
                break;
            case 4:
                d_r = t;
                d_g = p;
                d_b = color.v;
                break;
            case 5:
            default:
                d_r = color.v;
                d_g = p;
                d_b = q;
                break;
        }

        set_color(d_r, d_g, d_b);
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

    enum class SerializeMode : u8 { Hex, RGB, HSV };

    [[nodiscard]] std::string to_string(SerializeMode mode = SerializeMode::RGB) const;

    std::ostream& operator<<(std::ostream& os) const;
};
