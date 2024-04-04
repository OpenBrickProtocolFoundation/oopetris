
#include "helper/color.hpp"

#include <gtest/gtest.h>
#include <string>
#include <vector>

namespace {
    using ForeachCallback = std::function<void(u8 r, u8 g, u8 b)>;
    void foreach_loop(const ForeachCallback& callback) {
        u8 r{ 0 };
        u8 g{ 0 };
        u8 b{ 0 };
        do {
            do {
                do {
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

    const std::vector<std::string> valid_strings{ "#FFAA33",
                                                  "#FF00FF00"
                                                  "rgb(0,0,0)",
                                                  "rgba(0,0,0,0)"
                                                  "hsl(0,0,0)",
                                                  "hsla(9,9,9,10212)" };

    for (const auto& valid_string : valid_strings) {
        const auto result = Color::from_string(valid_string);
        EXPECT_TRUE(result.has_value());
    }
}

TEST(Color, FromStringInvalid) {

    const std::vector<std::string> invalid_strings{ "",          "#44",     "#Z",       "#ZZFF",      "u",
                                                    "#FFFFFFII", "#FFFF4T", "#0000001", "hsla(9,9,9)" };

    for (const auto& invalid_string : invalid_strings) {
        const auto result = Color::from_string(invalid_string);
        EXPECT_FALSE(result.has_value());
    }
}
