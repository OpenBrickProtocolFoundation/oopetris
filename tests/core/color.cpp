
#include <core/helper/color.hpp>
#include <core/helper/magic_enum_wrapper.hpp>

#include "utils/helper.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>
#include <vector>

namespace {
    using ForeachCallback = std::function<void(u8 red, u8 green, u8 blue)>;
    void foreach_loop(const ForeachCallback& callback) {
        u8 red{ 0 };
        u8 green{ 0 };
        u8 blue{ 0 };
        do {         // NOLINT(cppcoreguidelines-avoid-do-while)
            do {     // NOLINT(cppcoreguidelines-avoid-do-while)
                do { // NOLINT(cppcoreguidelines-avoid-do-while)
                    callback(red, green, blue);
                } while (red++ != 255);
            } while (green++ != 255);
        } while (blue++ != 255);
    }

} // namespace


// helper thought just for the tests
[[nodiscard]] constexpr bool
operator==(const HSVColor& value1, const HSVColor& value2) { //NOLINT(misc-use-internal-linkage)
    return value1.to_rgb_color() == value2.to_rgb_color();
}


// make colors printable
void PrintTo(const Color& color, std::ostream* os) { //NOLINT(misc-use-internal-linkage)
    *os << color.to_string();
}

void PrintTo(const HSVColor& color, std::ostream* os) { //NOLINT(misc-use-internal-linkage)
    *os << color.to_string();
}


namespace color {

    void PrintTo(const SerializeMode& value, std::ostream* os) { //NOLINT(misc-use-internal-linkage)
        *os << magic_enum::enum_name(value);
    }

}


TEST(Color, DefaultConstruction) {
    const auto color1 = Color{};
    const auto color2 = Color{ 0, 0, 0, 0 };
    ASSERT_EQ(color1, color2);
}

TEST(Color, ConstructorProperties) {
    foreach_loop([](u8 red, u8 green, u8 blue) {
        const auto color1 = Color{ red, green, blue };
        const auto color2 = Color{ red, green, blue, 0xFF };
        ASSERT_EQ(color1, color2);
    });
}

TEST(Color, FromStringValid) {

    const std::vector<std::tuple<std::string, Color::InfoType>> valid_values{
        {                   "#FFAA33",      { Color{ 0xFF, 0xAA, 0x33 }, color::SerializeMode::Hex, false } },
        {                 "#FF00FF00", { Color{ 0xFF, 0x00, 0xFF, 0x00 }, color::SerializeMode::Hex, true } },
        {                "rgb(0,0,0)",               { Color{ 0, 0, 0 }, color::SerializeMode::RGB, false } },
        {             "rgba(0,0,0,0)",             { Color{ 0, 0, 0, 0 }, color::SerializeMode::RGB, true } },
        {                "hsv(0,0,0)",            { HSVColor{ 0, 0, 0 }, color::SerializeMode::HSV, false } },
        {         "hsva(340,0,0.5,0)",      { HSVColor{ 340, 0, 0.5, 0 }, color::SerializeMode::HSV, true } },
        {                   "#ffaa33",      { Color{ 0xff, 0xaa, 0x33 }, color::SerializeMode::Hex, false } },
        { "hsv(0, 0.00_000_000_1, 0)",  { HSVColor{ 0, 0.000000001, 0 }, color::SerializeMode::HSV, false } },
        {       "hsva(0, 0, 0, 0xFF)",       { HSVColor{ 0, 0, 0, 0xFF }, color::SerializeMode::HSV, true } },
        {     "rgba(0, 0xFF, 0, 255)",        { Color{ 0, 0xFF, 0, 255 }, color::SerializeMode::RGB, true } },
        {   "rgba(0, 0xFF, 0, 1_0_0)",        { Color{ 0, 0xFF, 0, 100 }, color::SerializeMode::RGB, true } },
    };

    for (const auto& [valid_string, expected_result] : valid_values) {
        const auto result = Color::from_string_with_info(valid_string);
        ASSERT_THAT(result, ExpectedHasValue()) << "Input was: " << valid_string;
        ASSERT_EQ(result.value(), expected_result) << "Input was: " << valid_string;

        const auto color_string = std::get<0>(result.value()).to_string(color::SerializeMode::Hex);

        const auto converted_color = Color::from_string_with_info(color_string);
        ASSERT_THAT(converted_color, ExpectedHasValue()) << "Input was: " << color_string;
        ASSERT_EQ(result.value(), expected_result) << "Input was: " << color_string;
    }
}

TEST(Color, FromStringInvalid) {

    const std::vector<std::tuple<std::string, std::string>> invalid_strings{
        {                                          "", "not enough data to determine the literal type" },
        {                                       "#44",                      "Unrecognized HEX literal" },
        {                                        "#Z",                      "Unrecognized HEX literal" },
        {                                     "#ZZFF",                      "Unrecognized HEX literal" },
        {                                         "u",                    "Unrecognized color literal" },
        {                                 "#IIFFFFFF",       "the input must be a valid hex character" },
        {                                 "#FFIIFFFF",       "the input must be a valid hex character" },
        {                                 "#FFFFIIFF",       "the input must be a valid hex character" },
        {                                 "#FFFFFFII",       "the input must be a valid hex character" },
        {                                   "#FFFF4T",       "the input must be a valid hex character" },
        {                                  "#0000001",                      "Unrecognized HEX literal" },
        {                                "hsl(0,0,0)",                      "Unrecognized HSV literal" },
        {                                "rgg(0,0,0)",                      "Unrecognized RGB literal" },
        {                               "hsva(9,9,9)",                "s has to be in range 0.0 - 1.0" },
        {                         "hsva(9,9,9,10212)",                "s has to be in range 0.0 - 1.0" },
        {                               "hsv(-1,0,0)",   "the input must be a valid decimal character" },
        {                           "hsv(404040,0,0)",              "h has to be in range 0.0 - 360.0" },
        {                              "hsv(0,1.4,0)",                "s has to be in range 0.0 - 1.0" },
        {                              "hsv(0,0,1.7)",                "v has to be in range 0.0 - 1.0" },
        {                        "hsva(1321.4,0,0,0)",              "h has to be in range 0.0 - 360.0" },
        {                           "hsva(0,1.4,0,0)",                "s has to be in range 0.0 - 1.0" },
        {                           "hsva(0,0,1.7,0)",                "v has to be in range 0.0 - 1.0" },
        {                        "hsva(0, 0, 0, 256)",                  "a has to be in range 0 - 255" },
        {                            "hsv(0,0,1.7.8)",                        "only one comma allowed" },
        {                                     "hsv(0",                         "input ended too early" },
        {                    "rgba(0, 0xFFF, 0, 255)",                  "g has to be in range 0 - 255" },
        {                          "rgba(0, 0xFF, 0)",                                  "expected ','" },
        {                      "rgb(0, 0xFF, 0, 255)",                                  "expected ')'" },
        {                     "rgba(0, 0xFF, 0, 256)",                  "a has to be in range 0 - 255" },
        {                                    "rgba(0",                         "input ended too early" },
        {           "rgba(0, 0xFF, 0, 4_294_967_296)",                             "overflow detected" },
        {           "rgba(0, 0xFF, 0, 4_294_967_300)",                             "overflow detected" },
        { "rgba(0, 0xFF, 0, 121_123_124_294_967_300)",                             "overflow detected" },
        {                              "rgb(256,0,0)",                  "r has to be in range 0 - 255" },
        {                                    "rgb(0)",                                  "expected ','" },
        {                              "rgb(0,256,0)",                  "g has to be in range 0 - 255" },
        {                                  "rgb(0,0)",                                  "expected ','" },
        {                              "rgb(0,0,256)",                  "b has to be in range 0 - 255" },
        {                                "rgb(0,0,0,",                                  "expected ')'" },
        {                           "rgb(0,0,255)   ",                        "expected end of string" },
        {                           "rgba(256,0,0,0)",                  "r has to be in range 0 - 255" },
        {                                   "rgba(0)",                                  "expected ','" },
        {                           "rgba(0,256,0,0)",                  "g has to be in range 0 - 255" },
        {                                 "rgba(0,0)",                                  "expected ','" },
        {                           "rgba(0,0,256,0)",                  "b has to be in range 0 - 255" },
        {                               "rgba(0,0,0)",                                  "expected ','" },
        {                           "rgba(0,0,0,256)",                  "a has to be in range 0 - 255" },
        {                             "rgba(0,0,0,0,",                                  "expected ')'" },
        {                        "rgba(0,0,0,255)   ",                        "expected end of string" },
        {                                    "hsv(0)",                                  "expected ','" },
        {                                  "hsv(0,0)",                                  "expected ','" },
        {                                "hsv(0,0,0,",                                  "expected ')'" },
        {                             "hsv(0,0,0)   ",                        "expected end of string" },
        {                                   "hsva(0)",                                  "expected ','" },
        {                                 "hsva(0,0)",                                  "expected ','" },
        {                               "hsva(0,0,0)",                                  "expected ','" },
        {                             "hsva(0,0,0,0,",                                  "expected ')'" },
        {                        "hsva(0,0,0,255)   ",                        "expected end of string" },
    };

    for (const auto& [invalid_string, error_message] : invalid_strings) {
        const auto result = Color::from_string(invalid_string);
        ASSERT_THAT(result, ExpectedHasError()) << "Input was: " << invalid_string;
        ASSERT_EQ(result.error(), error_message) << "Input was: " << invalid_string;
    }
}


TEST(HSVColor, DefaultConstruction) {
    const auto color1 = HSVColor{};
    const auto color2 = HSVColor{ 0, 0, 0, 0 };
    ASSERT_EQ(color1, color2);
}

TEST(HSVColor, ConstructorProperties) {

    const std::vector<std::tuple<double, double, double>> values{
        {   0.0, 0.0, 0.0 },
        { 360.0, 0.0, 0.0 },
        { 360.0, 1.0, 0.0 },
        { 360.0, 1.0, 1.0 },
        {  57.0, 0.6, 0.8 }
    };

    for (const auto& [h, s, v] : values) {
        const auto color1 = HSVColor{ h, s, v };
        const auto color2 = HSVColor{ h, s, v, 0xFF };
        ASSERT_EQ(color1, color2);
    }
}

TEST(HSVColor, InvalidConstructors) {

    const std::vector<std::tuple<double, double, double>> invalid_values{
        {  -1.0,  0.0,  0.0 },
        { 360.0, -1.0,  0.0 },
        { 360.0,  1.0, -1.0 },
        { 460.0,  1.0,  1.0 },
        {  57.0,  2.6,  0.8 },
        {  57.0,  1.6,  3.8 }
    };

    for (const auto& [h, s, v] : invalid_values) {

        const auto construct = [h, s, v]() { HSVColor{ h, s, v }; }; //NOLINT(clang-analyzer-core.NullDereference)

        ASSERT_ANY_THROW(construct()); //NOLINT(*)
    }
}

#ifndef COLOR_TEST_MODE
#define COLOR_TEST_MODE 0
#endif


TEST(ColorConversion, HSVtoRGBtoHSV) {

#if COLOR_TEST_MODE == 0
    const std::vector<HSVColor> colors{
        HSVColor{   2, 0.3, 0.6 },
        HSVColor{  82, 0.3, 0.6 },
        HSVColor{ 142, 0.3, 0.6 },
        HSVColor{ 192, 0.3, 0.6 },
        HSVColor{ 252, 0.3, 0.6 },
        HSVColor{ 312, 0.3, 0.6 },
    };

    for (const auto& original_color : colors) {

#else
#if COLOR_TEST_MODE == 1
    constexpr const auto step_amount = 1000; // arbitrary number, to make it kinda exhaustive
#else
    constexpr const auto step_amount = COLOR_TEST_MODE;
#endif

    for (double h = 0.0; h < 360.0; h += 360.0 / step_amount) {
        for (double s = 0.0; s < 1.0; s += 1.0 / step_amount) {
            for (double v = 0.0; v < 1.0; v += 1.0 / step_amount) {
                const auto original_color = HSVColor{ h, s, v };


#endif
        const auto convert = [&original_color]() {
            const auto converted_color = original_color.to_rgb_color();

            const auto result_color = converted_color.to_hsv_color();

            ASSERT_EQ(original_color, result_color) << "Intermediate step: " << converted_color.to_string();
        };

        ASSERT_NO_THROW(convert()); //NOLINT(*)

#if COLOR_TEST_MODE != 0
    }
}
#endif
}
}


TEST(ColorConversion, RGGtoHSVtoRGB) {

#if COLOR_TEST_MODE == 0
    const std::vector<Color> colors{
        Color{   0,   0,   0 },
        Color{ 180, 135, 223 },
        Color{  12,  34, 130 },
        Color{  79,  85,  20 },
        Color{ 155, 174,   2 },
        Color{ 243,  32,  34 },
    };

    for (const auto& original_color : colors) {

#else

#if COLOR_TEST_MODE == 1
            constexpr const u8 step_amount = 0xFF; // max u8
#else

            constexpr const u8 step_amount = COLOR_TEST_MODE;
#endif

            constexpr const u8 u8_max = std::numeric_limits<u8>::max();
            constexpr const u8 step_size = u8_max / step_amount;


            for (u8 r = 0.0; u8_max - r < step_amount; r += step_size) {
                for (u8 g = 0.0; u8_max - g < step_amount; g += step_size) {
                    for (u8 b = 0.0; u8_max - b < step_amount; b += step_size) {
                        const auto original_color = Color{ r, g, b };
#endif
        const auto convert = [&original_color]() {
            const auto converted_color = original_color.to_hsv_color();

            const auto result_color = converted_color.to_rgb_color();

            ASSERT_EQ(original_color, result_color) << "Intermediate step: " << converted_color.to_string();
        };

        ASSERT_NO_THROW(convert()); //NOLINT(*)
#if COLOR_TEST_MODE != 0
    }
}
#endif
}
}
