

#include "color.hpp"
#include "color_literals.hpp"
#include "helper/expected.hpp"
#include "helper/utils.hpp"

#include <fmt/format.h>


#include <iostream>

[[nodiscard]] Color HSVColor::to_rgb_color() const {
    return Color{ *this };
}

[[nodiscard]] std::string HSVColor::to_string() const {
    return to_rgb_color().to_string(Color::SerializeMode::HSV);
}

helper::expected<Color, std::string> Color::from_string(const std::string& value) {

    const auto result = detail::get_color_from_string(value);

    if (result.has_value()) {
        return result.value();
    }

    return helper::unexpected<std::string>{ result.error() };
}

namespace {
    template<typename T>
    T fmod_always_positive(T value, T divisor) {
        if (value < static_cast<T>(0.0)) {
            // see https://math.stackexchange.com/questions/2179579/how-can-i-find-a-mod-with-negative-number
            T result = value;
            while (result < 0) {
                result += divisor;
            }
            return result;
        }
        return std::fmod(value, divisor);
    }
} // namespace

// taken carefully from https://math.stackexchange.com/questions/556341/rgb-to-hsv-color-conversion-algorithm
// and modified and sped up, by optimizing it manually
[[nodiscard]] HSVColor Color::to_hsv_color() const {
    constexpr auto max_d = static_cast<double>(0xFF);
    const double r_d = static_cast<double>(r) / max_d;
    const double g_d = static_cast<double>(g) / max_d;
    const double b_d = static_cast<double>(b) / max_d;

    const double min = std::min({ r_d, g_d, b_d });
    const double max = std::max({ r_d, g_d, b_d });
    const double delta = max - min;

    double h_temp = 0.0;
    if (min == max) {
        h_temp = 0.0;
    } else if (r >= max) {        // > is bogus, just keeps compiler happy
        h_temp = (g - b) / delta; // between yellow & magenta
    } else if (g >= max) {
        h_temp = 2.0 + ((b - r) / delta); // between cyan & yellow
    } else {
        h_temp = 4.0 + ((r - g) / delta); // between magenta & cyan
    }

    // use custom fmod, that always result in a positive value
    const double h = fmod_always_positive(h_temp, 6.0) * 60.0; //degrees

    const double s = max == 0.0 ? 0.0 : delta / max;

    //v = max

    return HSVColor{ h, s, max, a };
}

//Note: this output formats are all deserializable by the from_string method!
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
