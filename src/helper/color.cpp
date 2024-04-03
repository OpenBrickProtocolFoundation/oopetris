

#include "color.hpp"
#include "color_literals.hpp"
#include "helper/expected.hpp"
#include "helper/utils.hpp"

#include <fmt/format.h>

helper::expected<Color, std::string> Color::from_string(const std::string& value) {

    const auto result = detail::get_color_from_string(value);

    if (result.has_value()) {
        return result.value();
    }

    return helper::unexpected<std::string>{ result.error() };
}


[[nodiscard]] HSVColor Color::to_hsv_color() const {
    constexpr auto max_d = static_cast<double>(0xFF);
    const double r_d = static_cast<double>(r) / max_d;
    const double g_d = static_cast<double>(g) / max_d;
    const double b_d = static_cast<double>(b) / max_d;

    const double min = std::min({ r_d, g_d, b_d });
    const double max = std::max({ r_d, g_d, b_d });
    const double delta = max - min;

    double h = 0.0;

    if (r >= max) {                          // > is bogus, just keeps compiler happy
        h = std::fmod((g - b) / delta, 6.0); // between yellow & magenta
    } else if (g >= max) {
        h = 2.0 + ((b - r) / delta); // between cyan & yellow
    } else {
        h = 4.0 + ((r - g) / delta); // between magenta & cyan
    }

    //common factor, so just do it at the end
    h *= 60.0; // degrees

    const double s = max == 0.0 ? 0.0 : delta / max;

    const double v = max;

    return HSVColor{ h, s, v, a };
}

#if !defined(_NO_SDL)
[[nodiscard]] SDL_Color Color::to_sdl_color() const {
    return SDL_Color{ r, g, b, a };
}
#endif

//NOte: this output formats are all deserializable by the from_string method!
[[nodiscard]] std::string Color::to_string(SerializeMode mode) const {

    switch (mode) {
        case SerializeMode::Hex: {
            return fmt::format("#{:02x}{:02x}{:02x}{:02x}", r, g, b, a);
        }
        case SerializeMode::RGB: {
            return fmt::format("rgba({}, {}, {}, {:#2x})", r, g, b, a);
        }
        case SerializeMode::HSV: {
            const auto color = to_hsv_color();
            return fmt::format("hsva({:.2f}, {:.5f}, {:.5f}, {:#2x})", color.h, color.s, color.v, color.a);
        }
        default:
            utils::unreachable();
    }
}


std::ostream& Color::operator<<(std::ostream& os) const {
    os << to_string();
    return os;
}
