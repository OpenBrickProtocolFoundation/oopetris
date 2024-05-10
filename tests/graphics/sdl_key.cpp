
#include "manager/sdl_key.hpp"
#include "helper/expected.hpp"
#include "utils/helper.hpp"

#include "gtest/gtest.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <tuple>
#include <vector>


namespace SDL {

    // make keys printable
    void PrintTo(const Key& key, std::ostream* os) {
        *os << key.to_string();
    }
} // namespace SDL

TEST(SDLKey, SimpleComparision) {
    const auto key1 = SDL::Key{ SDLK_k };

    ASSERT_EQ(key1.to_string(), "K");
    ASSERT_EQ(key1.has_modifier(SDL::Modifier::ALT), false);
}

TEST(SDLKey, FromString) {

    const std::vector<std::tuple<helper::expected<SDL::Key, std::string>, std::string>> strings{
        {                         SDL::Key{ SDLK_1, { SDL::Modifier::CTRL } },    "Ctrl + 1" },
        {                       SDL::Key{ SDLK_1, { SDL::Modifier::LSHIFT } }, "Shift-L + 1" },
        {                       SDL::Key{ SDLK_1, { SDL::Modifier::RSHIFT } }, "Shift-R + 1" },
        { helper::unexpected<std::string>{ "Not a valid modifier: 'ShiftL'" },  "ShiftL + 1" },
     { helper::unexpected<std::string>{ "Duplicate modifier: 'Shift'" },  "Shift + Shift + 1" },
    };

    for (const auto& [correct, str] : strings) {

        const auto parsed = SDL::Key::from_string(str);

        if (correct.has_value()) {
            ASSERT_THAT(parsed, ExpectedHasValue()) << "Input was: " << str;
            ASSERT_EQ(correct.value(), parsed.value());
        } else {
            ASSERT_THAT(parsed, ExpectedHasError()) << "Input was: " << str;
            ASSERT_EQ(correct.error(), parsed.error());
        }
    }
}
