

#include "color.hpp"
#include "color_literals.hpp"
#include "helper/expected.hpp"
#include "helper/utils.hpp"

#include <fmt/format.h>


helper::expected<HSVColor, std::string> HSVColor::from_string(const std::string& value) {

    const auto result = detail::get_hsv_color_from_string(value);

    if (result.has_value()) {
        return std::get<0>(result.value());
    }

    return helper::unexpected<std::string>{ result.error() };
}

helper::expected<std::tuple<HSVColor, color::SerializeMode, bool>, std::string> HSVColor::from_string_with_info(
        const std::string& value
) {

    const auto result = detail::get_hsv_color_from_string(value);

    if (result.has_value()) {
        return result.value();
    }

    return helper::unexpected<std::string>{ result.error() };
}


[[nodiscard]] Color HSVColor::to_rgb_color() const {
    return Color{ *this };
}

[[nodiscard]] std::string HSVColor::to_string(bool force_alpha) const {

    const auto need_alpha = force_alpha || a != 0xFF;

    if (need_alpha) {
        return fmt::format("hsva({:.2f}, {:.5f}, {:.5f}, {:#2x})", h, s, v, a);
    }

    return fmt::format("hsv({:.2f}, {:.5f}, {:.5f})", h, s, v);
}


std::ostream& HSVColor::operator<<(std::ostream& os) const {
    os << to_string();
    return os;
}


helper::expected<Color, std::string> Color::from_string(const std::string& value) {

    const auto result = detail::get_color_from_string(value);

    if (result.has_value()) {
        return std::get<0>(result.value());
    }

    return helper::unexpected<std::string>{ result.error() };
}

helper::expected<std::tuple<Color, color::SerializeMode, bool>, std::string> Color::from_string_with_info(
        const std::string& value
) {

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
            //TODO: maybe this is possible faster?
            while (result < static_cast<T>(0.0)) {
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
    using FloatType = double; //for more precision use "long double" here

    constexpr auto max_d = static_cast<FloatType>(0xFF);
    const auto r_d = static_cast<FloatType>(r) / max_d;
    const auto g_d = static_cast<FloatType>(g) / max_d;
    const auto b_d = static_cast<FloatType>(b) / max_d;

    const auto min = std::min<FloatType>({ r_d, g_d, b_d });
    const auto max = std::max<FloatType>({ r_d, g_d, b_d });
    const auto delta = max - min;

    FloatType h_temp = 0.0;
    if (min == max) {
        h_temp = 0.0;
    } else if (r_d >= max) { // > is bogus, just keeps compiler happy
        h_temp = fmod_always_positive<FloatType>(
                (g_d - b_d) / delta, static_cast<FloatType>(6.0)
        ); // between yellow & magenta
    } else if (g_d >= max) {
        h_temp = 2.0 + ((b_d - r_d) / delta); // between cyan & yellow
    } else {
        h_temp = 4.0 + ((r_d - g_d) / delta); // between magenta & cyan
    }

    // use custom fmod, that always result in a positive value
    const FloatType h = h_temp * static_cast<FloatType>(60.0); //degrees

    const FloatType s = max == static_cast<FloatType>(0.0) ? static_cast<FloatType>(0.0) : delta / max;

    //v = max

    return HSVColor{ static_cast<double>(h), static_cast<double>(s), static_cast<double>(max), a };
}

//Note: this output formats are all deserializable by the from_string method!
[[nodiscard]] std::string Color::to_string(color::SerializeMode mode, bool force_alpha) const {

    const auto need_alpha = force_alpha || a != 0xFF;

    switch (mode) {
        case color::SerializeMode::Hex: {
            if (need_alpha) {
                return fmt::format("#{:02x}{:02x}{:02x}{:02x}", r, g, b, a);
            }

            return fmt::format("#{:02x}{:02x}{:02x}", r, g, b);
        }
        case color::SerializeMode::RGB: {
            if (need_alpha) {
                return fmt::format("rgba({}, {}, {}, {:#2x})", r, g, b, a);
            }

            return fmt::format("rgb({}, {}, {})", r, g, b);
        }
        case color::SerializeMode::HSV: {
            const auto color = to_hsv_color();
            return color.to_string(force_alpha);
        }
        default:
            utils::unreachable();
    }
}


std::ostream& Color::operator<<(std::ostream& os) const {
    os << to_string();
    return os;
}
