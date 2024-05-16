#pragma once

#include "color.hpp"
#include "const_utils.hpp"
#include "helper/types.hpp"

#include <limits>
#include <string>


namespace { //NOLINT(cert-dcl59-cpp,google-build-namespaces)

    namespace const_constants {

        // offsets in C strings for hex rgb and rgba
        constexpr std::size_t red_offset = 1;
        constexpr std::size_t green_offset = 3;
        constexpr std::size_t blue_offset = 5;
        constexpr std::size_t alpha_offset = 7;
        constexpr std::size_t hex_rgb_size = alpha_offset;
        constexpr std::size_t hex_rgba_size = 9;

    } // namespace const_constants

    // decode a decimal number
    [[nodiscard]] constexpr const_utils::expected<u8, std::string> single_decimal_number(char input) {
        if (input >= '0' && input <= '9') {
            return const_utils::expected<u8, std::string>::good_result(static_cast<u8>(input - '0'));
        }

        return const_utils::expected<u8, std::string>::error_result("the input must be a valid decimal character");
    }

    // decode a single_hex_number
    [[nodiscard]] constexpr const_utils::expected<u8, std::string> single_hex_number(char input) {
        if (input >= '0' && input <= '9') {
            return const_utils::expected<u8, std::string>::good_result(static_cast<u8>(input - '0'));
        }

        if (input >= 'A' && input <= 'F') {
            return const_utils::expected<u8, std::string>::good_result(static_cast<u8>(input - 'A' + 10));
        }

        if (input >= 'a' && input <= 'f') {
            return const_utils::expected<u8, std::string>::good_result(static_cast<u8>(input - 'a' + 10));
        }

        return const_utils::expected<u8, std::string>::error_result("the input must be a valid hex character");
    }


    //NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)

    // decode a single 2 digit color value in hex
    [[nodiscard]] constexpr const_utils::expected<u8, std::string> single_hex_color_value(const char* input) {

        const auto first = single_hex_number(input[0]);

        PROPAGATE(first, u8, std::string);

        const auto second = single_hex_number(input[1]);

        PROPAGATE(second, u8, std::string);

        return const_utils::expected<u8, std::string>::good_result((first.value() << 4) | second.value());
    }

    template<typename T>
    using CharIteratorResult = std::pair<T, const char*>;

    using DoubleReturnValue = CharIteratorResult<double>;

    // decode a single color value as double
    [[nodiscard]] constexpr const_utils::expected<DoubleReturnValue, std::string> single_double_color_value(
            const char* value
    ) {

        double result{ 0.0 };
        bool after_comma = false;
        double pow_of_10 = 1.0;

        for (std::size_t i = 0;; ++i) {

            const char current_char = value[i];

            switch (current_char) {
                case ' ':
                case '_':
                    break;
                case '.':
                    if (after_comma) {
                        return const_utils::expected<DoubleReturnValue, std::string>::error_result(
                                "only one comma allowed"
                        );
                    }
                    after_comma = true;
                    break;
                case ',':
                case ')':
                    return const_utils::expected<DoubleReturnValue, std::string>::good_result({ result, value + i });
                case '\0':
                    return const_utils::expected<DoubleReturnValue, std::string>::error_result("input ended too early");
                default: {

                    const auto char_result = single_decimal_number(current_char);

                    PROPAGATE(char_result, DoubleReturnValue, std::string);

                    const auto value_of_char = char_result.value();

                    if (after_comma) {
                        pow_of_10 *= 10.0;
                        result += static_cast<double>(value_of_char) / pow_of_10;
                    } else {
                        result = (result * 10.0) + static_cast<double>(value_of_char);
                    }
                    break;
                }
            }
        }

        return const_utils::expected<DoubleReturnValue, std::string>::error_result("unreachable");
    }

    using AnySizeType = u32;

    using AnyColorReturnValue = CharIteratorResult<AnySizeType>;

    // decode a single_hex_number
    [[nodiscard]] constexpr const_utils::expected<AnyColorReturnValue, std::string> single_color_value_any(
            const char* value
    ) {

        bool accept_hex = false;
        AnySizeType start = 0;
        AnySizeType mul_unit = 10;

        // skip leading white space
        while (value[start] == ' ') {
            ++start;
        }

        if (value[start] == '0' && value[start + 1] == 'x') {
            start += 2;
            accept_hex = true;
            mul_unit = 0x10;
        }


        AnySizeType result{ 0 };

        const auto max_value_before_multiplication = (std::numeric_limits<decltype(result)>::max() / mul_unit);

        const auto max_value_before_multiplication_rest =
                std::numeric_limits<decltype(result)>::max() - (max_value_before_multiplication * mul_unit);

        for (AnySizeType i = start;; ++i) {

            const char current_char = value[i];

            switch (current_char) {
                case ' ':
                case '_':
                    break;
                case ',':
                case ')':
                    return const_utils::expected<AnyColorReturnValue, std::string>::good_result({ result, value + i });
                case '\0':
                    return const_utils::expected<AnyColorReturnValue, std::string>::error_result("input ended too early"
                    );
                default: {

                    const auto char_result =
                            accept_hex ? single_hex_number(current_char) : single_decimal_number(current_char);

                    PROPAGATE(char_result, AnyColorReturnValue, std::string);

                    const auto value_of_char = char_result.value();

                    if (result == max_value_before_multiplication
                        && value_of_char > max_value_before_multiplication_rest) {
                        return const_utils::expected<AnyColorReturnValue, std::string>::error_result("overflow detected"
                        );
                    }

                    if (result > max_value_before_multiplication) {
                        return const_utils::expected<AnyColorReturnValue, std::string>::error_result("overflow detected"
                        );
                    }

                    result *= mul_unit;
                    result += value_of_char;
                    break;
                }
            }
        }

        return const_utils::expected<AnyColorReturnValue, std::string>::error_result("unreachable");
    }

    using ColorFromHexStringReturnType = std::pair<Color, bool>;

    [[nodiscard]] constexpr const_utils::expected<ColorFromHexStringReturnType, std::string>
    get_color_from_hex_string(const char* input, std::size_t size) {

        if (size == const_constants::hex_rgb_size) {

            const auto red = single_hex_color_value(input + const_constants::red_offset);
            PROPAGATE(red, ColorFromHexStringReturnType, std::string);

            const auto green = single_hex_color_value(input + const_constants::green_offset);
            PROPAGATE(green, ColorFromHexStringReturnType, std::string);

            const auto blue = single_hex_color_value(input + const_constants::blue_offset);
            PROPAGATE(blue, ColorFromHexStringReturnType, std::string);

            return const_utils::expected<ColorFromHexStringReturnType, std::string>::good_result({
                    Color{ red.value(), green.value(), blue.value() },
                    false
            });
        }

        if (size == const_constants::hex_rgba_size) {

            const auto red = single_hex_color_value(input + const_constants::red_offset);
            PROPAGATE(red, ColorFromHexStringReturnType, std::string);

            const auto green = single_hex_color_value(input + const_constants::green_offset);
            PROPAGATE(green, ColorFromHexStringReturnType, std::string);

            const auto blue = single_hex_color_value(input + const_constants::blue_offset);
            PROPAGATE(blue, ColorFromHexStringReturnType, std::string);

            const auto alpha = single_hex_color_value(input + const_constants::alpha_offset);
            PROPAGATE(alpha, ColorFromHexStringReturnType, std::string);

            return const_utils::expected<ColorFromHexStringReturnType, std::string>::good_result({
                    Color{ red.value(), green.value(), blue.value(), alpha.value() },
                    true
            });
        }


        return const_utils::expected<ColorFromHexStringReturnType, std::string>::error_result("Unrecognized HEX literal"
        );
    }

    using ColorFromRGBStringReturnType = std::pair<Color, bool>;

    [[nodiscard]] constexpr const_utils::expected<ColorFromRGBStringReturnType, std::string>
    get_color_from_rgb_string( //NOLINT(readability-function-cognitive-complexity)
            const char* input,
            std::size_t /*unused*/
    ) {

        if (input[0] == 'r' && input[1] == 'g' && input[2] == 'b') {
            if (input[3] == '(') {

                const auto r_result = single_color_value_any(input + 4);

                PROPAGATE(r_result, ColorFromRGBStringReturnType, std::string);

                const auto [r, next_g] = r_result.value();

                if (r > std::numeric_limits<u8>::max()) {
                    return const_utils::expected<ColorFromRGBStringReturnType, std::string>::error_result(
                            "r has to be in range 0 - 255"
                    );
                }

                if (*next_g != ',') {
                    return const_utils::expected<ColorFromRGBStringReturnType, std::string>::error_result("expected ','"
                    );
                }


                const auto g_result = single_color_value_any(next_g + 1);

                PROPAGATE(g_result, ColorFromRGBStringReturnType, std::string);

                const auto [g, next_b] = g_result.value();

                if (g > std::numeric_limits<u8>::max()) {
                    return const_utils::expected<ColorFromRGBStringReturnType, std::string>::error_result(
                            "g has to be in range 0 - 255"
                    );
                }

                if (*next_b != ',') {
                    return const_utils::expected<ColorFromRGBStringReturnType, std::string>::error_result("expected ','"
                    );
                }


                const auto b_result = single_color_value_any(next_b + 1);

                PROPAGATE(b_result, ColorFromRGBStringReturnType, std::string);

                const auto [b, end] = b_result.value();

                if (b > std::numeric_limits<u8>::max()) {
                    return const_utils::expected<ColorFromRGBStringReturnType, std::string>::error_result(
                            "b has to be in range 0 - 255"
                    );
                }

                if (*end != ')') {
                    return const_utils::expected<ColorFromRGBStringReturnType, std::string>::error_result("expected ')'"
                    );
                }

                if (*(end + 1) != '\0') {
                    return const_utils::expected<ColorFromRGBStringReturnType, std::string>::error_result(
                            "expected end of string"
                    );
                }


                return const_utils::expected<ColorFromRGBStringReturnType, std::string>::good_result({
                        Color{ static_cast<u8>(r), static_cast<u8>(g), static_cast<u8>(b) },
                        false
                });
            }


            if (input[3] == 'a' && input[4] == '(') {

                const auto r_result = single_color_value_any(input + 5);

                PROPAGATE(r_result, ColorFromRGBStringReturnType, std::string);

                const auto [r, next_g] = r_result.value();

                if (r > std::numeric_limits<u8>::max()) {
                    return const_utils::expected<ColorFromRGBStringReturnType, std::string>::error_result(
                            "r has to be in range 0 - 255"
                    );
                }

                if (*next_g != ',') {
                    return const_utils::expected<ColorFromRGBStringReturnType, std::string>::error_result("expected ','"
                    );
                }


                const auto g_result = single_color_value_any(next_g + 1);

                PROPAGATE(g_result, ColorFromRGBStringReturnType, std::string);

                const auto [g, next_b] = g_result.value();

                if (g > std::numeric_limits<u8>::max()) {
                    return const_utils::expected<ColorFromRGBStringReturnType, std::string>::error_result(
                            "g has to be in range 0 - 255"
                    );
                }

                if (*next_b != ',') {
                    return const_utils::expected<ColorFromRGBStringReturnType, std::string>::error_result("expected ','"
                    );
                }


                const auto b_result = single_color_value_any(next_b + 1);

                PROPAGATE(b_result, ColorFromRGBStringReturnType, std::string);

                const auto [b, next_a] = b_result.value();

                if (b > std::numeric_limits<u8>::max()) {
                    return const_utils::expected<ColorFromRGBStringReturnType, std::string>::error_result(
                            "b has to be in range 0 - 255"
                    );
                }

                if (*next_a != ',') {
                    return const_utils::expected<ColorFromRGBStringReturnType, std::string>::error_result("expected ','"
                    );
                }

                const auto a_result = single_color_value_any(next_a + 1);

                PROPAGATE(a_result, ColorFromRGBStringReturnType, std::string);

                const auto [a, end] = a_result.value();

                if (a > std::numeric_limits<u8>::max()) {
                    return const_utils::expected<ColorFromRGBStringReturnType, std::string>::error_result(
                            "a has to be in range 0 - 255"
                    );
                }


                if (*end != ')') {
                    return const_utils::expected<ColorFromRGBStringReturnType, std::string>::error_result("expected ')'"
                    );
                }

                if (*(end + 1) != '\0') {
                    return const_utils::expected<ColorFromRGBStringReturnType, std::string>::error_result(
                            "expected end of string"
                    );
                }


                return const_utils::expected<ColorFromRGBStringReturnType, std::string>::good_result({
                        Color{ static_cast<u8>(r), static_cast<u8>(g), static_cast<u8>(b), static_cast<u8>(a) },
                        true
                }
                );
            }
        }


        return const_utils::expected<ColorFromRGBStringReturnType, std::string>::error_result("Unrecognized RGB literal"
        );
    }

    using ColorFromHSVStringReturnType = std::pair<HSVColor, bool>;

    [[nodiscard]] constexpr const_utils::expected<ColorFromHSVStringReturnType, std::string>
    get_color_from_hsv_string( //NOLINT(readability-function-cognitive-complexity)
            const char* input,
            std::size_t /*unused*/
    ) {

        if (input[0] == 'h' && input[1] == 's' && input[2] == 'v') {
            if (input[3] == '(') {

                const auto h_result = single_double_color_value(input + 4);

                PROPAGATE(h_result, ColorFromHSVStringReturnType, std::string);

                const auto [h, next_s] = h_result.value();

                if (h < 0.0 || h > 360.0) {
                    return const_utils::expected<ColorFromHSVStringReturnType, std::string>::error_result(
                            "h has to be in range 0.0 - 360.0"
                    );
                }

                if (*next_s != ',') {
                    return const_utils::expected<ColorFromHSVStringReturnType, std::string>::error_result("expected ','"
                    );
                }


                const auto s_result = single_double_color_value(next_s + 1);

                PROPAGATE(s_result, ColorFromHSVStringReturnType, std::string);

                const auto [s, next_v] = s_result.value();

                if (s < 0.0 || s > 1.0) {
                    return const_utils::expected<ColorFromHSVStringReturnType, std::string>::error_result(
                            "s has to be in range 0.0 - 1.0"
                    );
                }

                if (*next_v != ',') {
                    return const_utils::expected<ColorFromHSVStringReturnType, std::string>::error_result("expected ','"
                    );
                }


                const auto v_result = single_double_color_value(next_v + 1);

                PROPAGATE(v_result, ColorFromHSVStringReturnType, std::string);

                const auto [v, end] = v_result.value();

                if (v < 0.0 || v > 1.0) {
                    return const_utils::expected<ColorFromHSVStringReturnType, std::string>::error_result(
                            "v has to be in range 0.0 - 1.0"
                    );
                }

                if (*end != ')') {
                    return const_utils::expected<ColorFromHSVStringReturnType, std::string>::error_result("expected ')'"
                    );
                }

                if (*(end + 1) != '\0') {
                    return const_utils::expected<ColorFromHSVStringReturnType, std::string>::error_result(
                            "expected end of string"
                    );
                }


                return const_utils::expected<ColorFromHSVStringReturnType, std::string>::good_result({
                        HSVColor{ h, s, v },
                        false
                });
            }


            if (input[3] == 'a' && input[4] == '(') {

                const auto h_result = single_double_color_value(input + 5);

                PROPAGATE(h_result, ColorFromHSVStringReturnType, std::string);

                const auto [h, next_s] = h_result.value();

                if (h < 0.0 || h > 360.0) {
                    return const_utils::expected<ColorFromHSVStringReturnType, std::string>::error_result(
                            "h has to be in range 0.0 - 360.0"
                    );
                }

                if (*next_s != ',') {
                    return const_utils::expected<ColorFromHSVStringReturnType, std::string>::error_result("expected ','"
                    );
                }


                const auto s_result = single_double_color_value(next_s + 1);

                PROPAGATE(s_result, ColorFromHSVStringReturnType, std::string);

                const auto [s, next_v] = s_result.value();

                if (s < 0.0 || s > 1.0) {
                    return const_utils::expected<ColorFromHSVStringReturnType, std::string>::error_result(
                            "s has to be in range 0.0 - 1.0"
                    );
                }

                if (*next_v != ',') {
                    return const_utils::expected<ColorFromHSVStringReturnType, std::string>::error_result("expected ','"
                    );
                }


                const auto v_result = single_double_color_value(next_v + 1);

                PROPAGATE(v_result, ColorFromHSVStringReturnType, std::string);

                const auto [v, next_a] = v_result.value();

                if (v < 0.0 || v > 1.0) {
                    return const_utils::expected<ColorFromHSVStringReturnType, std::string>::error_result(
                            "v has to be in range 0.0 - 1.0"
                    );
                }

                if (*next_a != ',') {
                    return const_utils::expected<ColorFromHSVStringReturnType, std::string>::error_result("expected ','"
                    );
                }

                const auto a_result = single_color_value_any(next_a + 1);

                PROPAGATE(a_result, ColorFromHSVStringReturnType, std::string);

                const auto [a, end] = a_result.value();

                if (a > std::numeric_limits<u8>::max()) {
                    return const_utils::expected<ColorFromHSVStringReturnType, std::string>::error_result(
                            "a has to be in range 0 - 255"
                    );
                }


                if (*end != ')') {
                    return const_utils::expected<ColorFromHSVStringReturnType, std::string>::error_result("expected ')'"
                    );
                }

                if (*(end + 1) != '\0') {
                    return const_utils::expected<ColorFromHSVStringReturnType, std::string>::error_result(
                            "expected end of string"
                    );
                }


                return const_utils::expected<ColorFromHSVStringReturnType, std::string>::good_result({
                        HSVColor{ h, s, v, static_cast<u8>(a) },
                        true
                });
            }
        }


        return const_utils::expected<ColorFromHSVStringReturnType, std::string>::error_result("Unrecognized HSV literal"
        );
    }

    using ColorFromStringReturnType = std::tuple<Color, color::SerializeMode, bool>;

    [[nodiscard]] constexpr const_utils::expected<ColorFromStringReturnType, std::string>
    get_color_from_string_impl(const char* input, std::size_t size) {

        if (size == 0) {
            return const_utils::expected<ColorFromStringReturnType, std::string>::error_result(
                    "not enough data to determine the literal type"
            );
        }

        switch (input[0]) {
            case '#': {
                const auto result = get_color_from_hex_string(input, size);

                PROPAGATE(result, ColorFromStringReturnType, std::string);

                const auto value = result.value();


                return const_utils::expected<ColorFromStringReturnType, std::string>::good_result(
                        { value.first, color::SerializeMode::Hex, value.second }
                );
            }
            case 'r': {
                const auto result = get_color_from_rgb_string(input, size);

                PROPAGATE(result, ColorFromStringReturnType, std::string);

                const auto value = result.value();


                return const_utils::expected<ColorFromStringReturnType, std::string>::good_result(
                        { value.first, color::SerializeMode::RGB, value.second }
                );
            }
            case 'h': {
                const auto result = get_color_from_hsv_string(input, size);

                PROPAGATE(result, ColorFromStringReturnType, std::string);

                const auto value = result.value();


                return const_utils::expected<ColorFromStringReturnType, std::string>::good_result(
                        { Color{ value.first }, color::SerializeMode::HSV, value.second }
                );
            }
            default:
                return const_utils::expected<ColorFromStringReturnType, std::string>::error_result(
                        "Unrecognized color literal"
                );
        }
    }

    using HSVColorFromStringReturnType = std::tuple<HSVColor, color::SerializeMode, bool>;

    [[nodiscard]] constexpr const_utils::expected<HSVColorFromStringReturnType, std::string>
    get_hsv_color_from_string_impl(const char* input, std::size_t size) {

        if (size == 0) {
            return const_utils::expected<HSVColorFromStringReturnType, std::string>::error_result(
                    "not enough data to determine the literal type"
            );
        }

        switch (input[0]) {
            case '#': {
                const auto result = get_color_from_hex_string(input, size);

                PROPAGATE(result, HSVColorFromStringReturnType, std::string);

                const auto value = result.value();

                return const_utils::expected<HSVColorFromStringReturnType, std::string>::good_result(
                        { value.first.to_hsv_color(), color::SerializeMode::Hex, value.second }
                );
            }
            case 'r': {
                const auto result = get_color_from_rgb_string(input, size);

                PROPAGATE(result, HSVColorFromStringReturnType, std::string);

                const auto value = result.value();

                return const_utils::expected<HSVColorFromStringReturnType, std::string>::good_result(
                        { value.first.to_hsv_color(), color::SerializeMode::RGB, value.second }
                );
            }
            case 'h': {
                const auto result = get_color_from_hsv_string(input, size);

                PROPAGATE(result, HSVColorFromStringReturnType, std::string);

                const auto value = result.value();

                return const_utils::expected<HSVColorFromStringReturnType, std::string>::good_result(
                        { value.first, color::SerializeMode::HSV, value.second }
                );
            }
            default:
                return const_utils::expected<HSVColorFromStringReturnType, std::string>::error_result(
                        "Unrecognized color literal"
                );
        }
    }

    //NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)
} // namespace


namespace detail {

    [[nodiscard]] constexpr const_utils::expected<ColorFromStringReturnType, std::string> get_color_from_string(
            const std::string& input
    ) {
        return get_color_from_string_impl(input.c_str(), input.size());
    }

    [[nodiscard]] constexpr const_utils::expected<HSVColorFromStringReturnType, std::string> get_hsv_color_from_string(
            const std::string& input
    ) {
        return get_hsv_color_from_string_impl(input.c_str(), input.size());
    }

} // namespace detail

consteval Color operator""_c(const char* input, std::size_t size) {
    const auto result = get_color_from_string_impl(input, size);

    CONSTEVAL_STATIC_ASSERT(result.has_value(), "incorrect color literal");

    return std::get<0>(result.value());
}

consteval HSVColor operator""_hsv(const char* input, std::size_t size) {
    const auto result = get_hsv_color_from_string_impl(input, size);

    CONSTEVAL_STATIC_ASSERT(result.has_value(), "incorrect color literal");

    return std::get<0>(result.value());
}


// sanity tests at compile time:

static_assert("#ABCDEF"_c == "#ABCDEFFF"_c);
static_assert("hsv(0, 0, 0)"_c == "hsva(0, 0, 0, 0xFF)"_c);
static_assert("rgb(0, 0, 0)"_c == "rgba(0, 0, 0, 0xFF)"_c);
static_assert("rgb(0xAB, 0xCD, 0xEF)"_c == "rgb(171, 205, 239)"_c);
static_assert("hsv(0, 0, 0.79)"_c == "#C9C9C9"_c);


// due to an error in MSVC, the "#FFFFFF"_c literal, can't be constants, so juts using the runtime variant
// (which has a optional access without check, but the CI detects errors in those strings on other platforms and compilers at compiler time,
// so that's not that bad)
#if defined(_MSC_VER)
#define COLOR_LITERAL(color_string) Color::from_string(color_string).value() //NOLINT(cppcoreguidelines-macro-usage)
#else
#define COLOR_LITERAL(color_string) color_string##_c //NOLINT(cppcoreguidelines-macro-usage)
#endif
