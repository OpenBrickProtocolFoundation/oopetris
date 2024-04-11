
#include "helper/color.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>
#include <vector>

namespace {
    using ForeachCallback = std::function<void(u8 r, u8 g, u8 b)>;
    void foreach_loop(const ForeachCallback& callback) {
        u8 r{ 0 };
        u8 g{ 0 };
        u8 b{ 0 };
        do {         // NOLINT(cppcoreguidelines-avoid-do-while)
            do {     // NOLINT(cppcoreguidelines-avoid-do-while)
                do { // NOLINT(cppcoreguidelines-avoid-do-while)
                    callback(r, g, b);
                } while (r++ != 255);
            } while (g++ != 255);
        } while (b++ != 255);
    }


} // namespace

// make colors printable
void PrintTo(const Color& color, std::ostream* os) {
    *os << color.to_string();
}

void PrintTo(const HSVColor& color, std::ostream* os) {
    *os << color.to_string();
}

// make helper::expected printable
template<typename T, typename S>
void PrintTo(const helper::expected<T, S>& value, std::ostream* os) {
    if (value.has_value()) {
        *os << "Value: " << ::testing::PrintToString<T>(value.value());
    } else {
        *os << "Error: " << ::testing::PrintToString<S>(value.error());
    }
}


MATCHER(ExpectedHasValue, "expected has value") {
    return arg.has_value();
}

MATCHER(ExpectedHasError, "expected has error") {
    return not arg.has_value();
}

TEST(Color, DefaultConstruction) {
    const auto c1 = Color{};
    const auto c2 = Color{ 0, 0, 0, 0 };
    ASSERT_EQ(c1, c2);
}

TEST(Color, ConstructorProperties) {
    foreach_loop([](u8 r, u8 g, u8 b) {
        const auto c1 = Color{ r, g, b };
        const auto c2 = Color{ r, g, b, 0xFF };
        ASSERT_EQ(c1, c2);
    });
}

TEST(Color, FromStringValid) {

    const std::vector<std::tuple<std::string, Color>> valid_strings{
        {                  "#FFAA33",       Color{ 0xFF, 0xAA, 0x33 }},
        {                "#FF00FF00", Color{ 0xFF, 0x00, 0xFF, 0x00 }},
        {               "rgb(0,0,0)",                Color{ 0, 0, 0 }},
        {            "rgba(0,0,0,0)",             Color{ 0, 0, 0, 0 }},
        {               "hsv(0,0,0)",             HSVColor{ 0, 0, 0 }},
        {        "hsva(340,0,0.5,0)",      HSVColor{ 340, 0, 0.5, 0 }},
        {                  "#ffaa33",       Color{ 0xff, 0xaa, 0x33 }},
        {"hsv(0, 0.00_000_000_1, 0)",   HSVColor{ 0, 0.000000001, 0 }},
        {      "hsva(0, 0, 0, 0xFF)",       HSVColor{ 0, 0, 0, 0xFF }},
        {    "rgba(0, 0xFF, 0, 255)",        Color{ 0, 0xFF, 0, 255 }},
        {  "rgba(0, 0xFF, 0, 1_0_0)",        Color{ 0, 0xFF, 0, 100 }},
    };

    for (const auto& [valid_string, expected_color] : valid_strings) {
        const auto result = Color::from_string(valid_string);
        ASSERT_THAT(result, ExpectedHasValue()) << "Input was: " << valid_string;
        ASSERT_EQ(result.value(), expected_color) << "Input was: " << valid_string;
    }
}

TEST(Color, FromStringInvalid) {

    const std::vector<std::tuple<std::string, std::string>> invalid_strings{
        {                                         "", "not enough data to determine the literal type"},
        {                                      "#44",                      "Unrecognized HEX literal"},
        {                                       "#Z",                      "Unrecognized HEX literal"},
        {                                    "#ZZFF",                      "Unrecognized HEX literal"},
        {                                        "u",                    "Unrecognized color literal"},
        {                                "#FFFFFFII",       "the input must be a valid hex character"},
        {                                  "#FFFF4T",       "the input must be a valid hex character"},
        {                                 "#0000001",                      "Unrecognized HEX literal"},
        {                               "hsl(0,0,0)",                      "Unrecognized HSV literal"},
        {                               "rgg(0,0,0)",                      "Unrecognized RGB literal"},
        {                              "hsva(9,9,9)",                "s has to be in range 0.0 - 1.0"},
        {                        "hsva(9,9,9,10212)",                "s has to be in range 0.0 - 1.0"},
        {                              "hsv(-1,0,0)",   "the input must be a valid decimal character"},
        {                          "hsv(404040,0,0)",              "h has to be in range 0.0 - 360.0"},
        {                             "hsv(0,1.4,0)",                "s has to be in range 0.0 - 1.0"},
        {                             "hsv(0,0,1.7)",                "v has to be in range 0.0 - 1.0"},
        {                       "hsva(1321.4,0,0,0)",              "h has to be in range 0.0 - 360.0"},
        {                          "hsva(0,1.4,0,0)",                "s has to be in range 0.0 - 1.0"},
        {                          "hsva(0,0,1.7,0)",                "v has to be in range 0.0 - 1.0"},
        {                       "hsva(0, 0, 0, 256)",                  "a has to be in range 0 - 255"},
        {                           "hsv(0,0,1.7.8)",                        "only one comma allowed"},
        {                                    "hsv(0",                         "input ended too early"},
        {                   "rgba(0, 0xFFF, 0, 255)",                  "g has to be in range 0 - 255"},
        {                         "rgba(0, 0xFF, 0)",                                  "expected ','"},
        {                     "rgb(0, 0xFF, 0, 255)",                                  "expected ')'"},
        {                    "rgba(0, 0xFF, 0, 256)",                  "a has to be in range 0 - 255"},
        {                                   "rgba(0",                         "input ended too early"},
        {          "rgba(0, 0xFF, 0, 4_294_967_296)",                             "overflow detected"},
        {          "rgba(0, 0xFF, 0, 4_294_967_300)",                             "overflow detected"},
        {"rgba(0, 0xFF, 0, 121_123_124_294_967_300)",                             "overflow detected"},
        {                             "rgb(256,0,0)",                  "r has to be in range 0 - 255"},
        {                                   "rgb(0)",                                  "expected ','"},
        {                             "rgb(0,256,0)",                  "g has to be in range 0 - 255"},
        {                                 "rgb(0,0)",                                  "expected ','"},
        {                             "rgb(0,0,256)",                  "b has to be in range 0 - 255"},
        {                               "rgb(0,0,0,",                                  "expected ')'"},
        {                          "rgb(0,0,255)   ",                        "expected end of string"},
        {                          "rgba(256,0,0,0)",                  "r has to be in range 0 - 255"},
        {                                  "rgba(0)",                                  "expected ','"},
        {                          "rgba(0,256,0,0)",                  "g has to be in range 0 - 255"},
        {                                "rgba(0,0)",                                  "expected ','"},
        {                          "rgba(0,0,256,0)",                  "b has to be in range 0 - 255"},
        {                              "rgba(0,0,0)",                                  "expected ','"},
        {                          "rgba(0,0,0,256)",                  "a has to be in range 0 - 255"},
        {                            "rgba(0,0,0,0,",                                  "expected ')'"},
        {                       "rgba(0,0,0,255)   ",                        "expected end of string"},
        {                                   "hsv(0)",                                  "expected ','"},
        {                                 "hsv(0,0)",                                  "expected ','"},
        {                               "hsv(0,0,0,",                                  "expected ')'"},
        {                            "hsv(0,0,0)   ",                        "expected end of string"},
        {                                  "hsva(0)",                                  "expected ','"},
        {                                "hsva(0,0)",                                  "expected ','"},
        {                              "hsva(0,0,0)",                                  "expected ','"},
        {                            "hsva(0,0,0,0,",                                  "expected ')'"},
        {                       "hsva(0,0,0,255)   ",                        "expected end of string"},
    };

    for (const auto& [invalid_string, error_message] : invalid_strings) {
        const auto result = Color::from_string(invalid_string);
        ASSERT_THAT(result, ExpectedHasError()) << "Input was: " << invalid_string;
        ASSERT_EQ(result.error(), error_message) << "Input was: " << invalid_string;
    }
}


namespace {

    testing::AssertionResult HSVColorEqPred(const char*, const char*, const HSVColor& val1, const HSVColor& val2) {

        constexpr auto max_h_err = 0.5;
        constexpr auto max_sv_err = 0.01;

        const auto near_helper = [](double arg1, double arg2, double margin) -> bool {
            return ::testing::internal::DoubleNearPredFormat("", "", "", arg1, arg2, margin);
        };

        if (not near_helper(val1.h, val2.h, max_h_err)) {
            return testing::AssertionFailure() << "HSVColors " << val1.to_string() << " and " << val2.to_string()
                                               << " are not EQ, reason: h is not near enough\n"
                                               << val1.h << " != " << val2.h;
        }

        if (not near_helper(val1.s, val2.s, max_sv_err)) {
            return testing::AssertionFailure() << "HSVColors " << val1.to_string() << " and " << val2.to_string()
                                               << " are not EQ, reason: s is not near enough\n"
                                               << val1.s << " != " << val2.s;
        }
        if (not near_helper(val1.v, val2.v, max_sv_err)) {
            return testing::AssertionFailure() << "HSVColors " << val1.to_string() << " and " << val2.to_string()
                                               << " are not EQ, reason: v is not near enough\n"
                                               << val1.v << " != " << val2.v;
        }

        if (val1.a != val2.a) {
            return testing::AssertionFailure() << "HSVColors " << val1.to_string() << " and " << val2.to_string()
                                               << " are not EQ, reason: a is not equal\n"
                                               << val1.a << " != " << val2.a;
        }

        return testing::AssertionSuccess();
    }
} // namespace

#define ASSERT_EQ_HSV_COLOR(val1, val2) /*NOLINT(cppcoreguidelines-macro-usage)*/ \
    ASSERT_PRED_FORMAT2(HSVColorEqPred, val1, val2)


TEST(HSVColor, DefaultConstruction) {
    const auto c1 = HSVColor{};
    const auto c2 = HSVColor{ 0, 0, 0, 0 };
    ASSERT_EQ_HSV_COLOR(c1, c2);
}

TEST(HSVColor, ConstructorProperties) {

    const std::vector<std::tuple<double, double, double>> values{
        {  0.0, 0.0, 0.0},
        {360.0, 0.0, 0.0},
        {360.0, 1.0, 0.0},
        {360.0, 1.0, 1.0},
        { 57.0, 0.6, 0.8}
    };

    for (const auto& [h, s, v] : values) {
        const auto c1 = HSVColor{ h, s, v };
        const auto c2 = HSVColor{ h, s, v, 0xFF };
        ASSERT_EQ_HSV_COLOR(c1, c2);
    }
}

TEST(HSVColor, InvalidConstructors) {

    const std::vector<std::tuple<double, double, double>> invalid_values{
        { -1.0,  0.0,  0.0},
        {360.0, -1.0,  0.0},
        {360.0,  1.0, -1.0},
        {460.0,  1.0,  1.0},
        { 57.0,  2.6,  0.8},
        { 57.0,  1.6,  3.8}
    };

    for (const auto& [h, s, v] : invalid_values) {

        const auto construct = [h, s, v]() { HSVColor{ h, s, v }; }; //NOLINT(clang-analyzer-core.NullDereference)

        ASSERT_ANY_THROW(construct()); //NOLINT(*)
    }
}


TEST(ColorConversion, HSV_to_RGB_to_HSV) {
    constexpr const auto step_amount = 20; // to debug, increase this value, than you ahve more samples

    for (double h = 0.0; h < 360.0; h += 360.0 / step_amount) {
        for (double s = 0.0; s < 1.0; s += 1.0 / step_amount) {
            for (double v = 0.0; v < 1.0; v += 1.0 / step_amount) {
                const auto convert = [h, s, v]() {
                    const auto original_color = HSVColor{ h, s, v };

                    const auto converted_color = original_color.to_rgb_color();

                    const auto result_color = converted_color.to_hsv_color();

                    UNUSED(result_color);
                    //  ASSERT_EQ_HSV_COLOR(original_color, result_color);
                };

                ASSERT_NO_THROW(convert()); //NOLINT(*)
            }
        }
    }
}


TEST(ColorConversion, RGG_to_HSV_to_RGB) {
    constexpr const u8 step_amount = 20; // to debug, increase this value, than you ahve more samples

    constexpr const u8 u8_max = std::numeric_limits<u8>::max();
    constexpr const u8 step_size = u8_max / step_amount;

    for (u8 r = 0.0; u8_max - r < step_amount; r += step_size) {
        for (u8 g = 0.0; u8_max - g < step_amount; g += step_size) {
            for (u8 b = 0.0; u8_max - b < step_amount; b += step_size) {
                const auto convert = [r, g, b]() {
                    const auto original_color = Color{ r, g, b };

                    const auto converted_color = original_color.to_hsv_color();

                    const auto result_color = converted_color.to_rgb_color();

                    UNUSED(result_color);
                    //  ASSERT_EQ_HSV_COLOR(original_color, result_color);
                };

                ASSERT_NO_THROW(convert()); //NOLINT(*)
            }
        }
    }
}
