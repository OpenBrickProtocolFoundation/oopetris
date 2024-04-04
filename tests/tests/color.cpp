
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
void PrintTo(const Color& point, std::ostream* os) {
    *os << point.to_string();
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

MATCHER(ExpectedHasError, "expected has value") {
    return not arg.has_value();
}

TEST(Color, DefaultConstruction) {
    const auto c1 = Color{};
    const auto c2 = Color{ 0, 0, 0, 0 };
    EXPECT_EQ(c1, c2);
}

TEST(Color, ConstructorProperties) {
    foreach_loop([](u8 r, u8 g, u8 b) {
        const auto c1 = Color{ r, g, b };
        const auto c2 = Color{ r, g, b, 0xFF };
        EXPECT_EQ(c1, c2);
    });
}

TEST(Color, FromStringValid) {

    const std::vector<std::string> valid_strings{ "#FFAA33",       "#FF00FF00",  "rgb(0,0,0)",
                                                  "rgba(0,0,0,0)", "hsv(0,0,0)", "hsva(340,0,0.5,0)" };

    for (const auto& valid_string : valid_strings) {
        const auto result = Color::from_string(valid_string);
        EXPECT_THAT(result, ExpectedHasValue()) << "Input was: " << valid_string;
    }
}

TEST(Color, FromStringInvalid) {

    const std::vector<std::string> invalid_strings{ "",
                                                    "#44",
                                                    "#Z",
                                                    "#ZZFF",
                                                    "u",
                                                    "#FFFFFFII",
                                                    "#FFFF4T",
                                                    "#0000001",
                                                    "hsl(0,0,0)",
                                                    "hsva(9,9,9)",
                                                    "hsva(9,9,9,10212)",
                                                    "hsv(-1,0,0)",
                                                    "hsv(404040,0,0)"
                                                    "hsv(0,1.4,0)",
                                                    "hsv(0,0,1.7)" };

    for (const auto& invalid_string : invalid_strings) {
        const auto result = Color::from_string(invalid_string);
        EXPECT_THAT(result, ExpectedHasError()) << "Input was: " << invalid_string;
    }
}


TEST(HSVColor, DefaultConstruction) {
    const auto c1 = HSVColor{};
    const auto c2 = HSVColor{ 0, 0, 0, 0 };
    EXPECT_EQ(c1, c2);
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
        EXPECT_EQ(c1, c2);
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

        const auto construct = [h, s, v]() { HSVColor{ h, s, v }; };

        EXPECT_ANY_THROW(construct());
    }
}
