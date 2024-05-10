
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


    std::ostream& operator<<(std::ostream& os, const Key& value) {
        os << value.to_string();
        return os;
    }
} // namespace SDL

TEST(SDLKey, SimpleComparision) {
    const auto key1 = SDL::Key{ SDLK_k };

    ASSERT_EQ(key1.to_string(), "K");
    ASSERT_EQ(key1.has_modifier(SDL::Modifier::ALT), false);
}

TEST(SDLKey, FromString) {

    const std::vector<std::tuple<helper::expected<SDL::Key, std::string>, std::string>> strings{
        {                                                               SDL::Key{ SDLK_1, { SDL::Modifier::CTRL } },"Ctrl + 1"                                                                                                                    },
        {                                                             SDL::Key{ SDLK_1, { SDL::Modifier::LSHIFT } },       "Shift-L + 1" },
        {                                                             SDL::Key{ SDLK_1, { SDL::Modifier::RSHIFT } },       "Shift-R + 1" },
        {                                       helper::unexpected<std::string>{ "Not a valid modifier: 'ShiftL'" },        "ShiftL + 1" },
        {                                          helper::unexpected<std::string>{ "Duplicate modifier: 'Shift'" }, "Shift + Shift + 1" },
        {                                      helper::unexpected<std::string>{ "No key but only modifiers given" },             "Shift" },
        {                                                          helper::unexpected<std::string>{ "Empty token" },                  "" },
        {                                                          helper::unexpected<std::string>{ "Empty token" },         "   + \t  " },
        {                                                                                        SDL::Key{ SDLK_1 },                 "1" },
        { SDL::Key{ SDLK_1, { SDL::Modifier::CTRL, SDL::Modifier::ALT, SDL::Modifier::SHIFT, SDL::Modifier::GUI } },
         "Shift + Alt + Ctrl + Gui + 1"                                                                                                 },
        {                                                              SDL::Key{ SDLK_1, { SDL::Modifier::LCTRL } },        "Ctrl-L + 1" },
        {                                                              SDL::Key{ SDLK_1, { SDL::Modifier::RCTRL } },        "Ctrl-R + 1" },
        {                                                               SDL::Key{ SDLK_1, { SDL::Modifier::LALT } },         "Alt-L + 1" },
        {                                                               SDL::Key{ SDLK_1, { SDL::Modifier::RALT } },         "Alt-R + 1" },
        {                                                               SDL::Key{ SDLK_1, { SDL::Modifier::LGUI } },         "Gui-L + 1" },
        {                                                               SDL::Key{ SDLK_1, { SDL::Modifier::RGUI } },         "Gui-R + 1" },
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


TEST(SDLKey, ToString) {

    const std::vector<SDL::Key> keys{
        SDL::Key{ SDLK_1, { SDL::Modifier::CTRL } },
        SDL::Key{ SDLK_1, { SDL::Modifier::LSHIFT } },
        SDL::Key{ SDLK_1, { SDL::Modifier::RSHIFT } },
        SDL::Key{ SDLK_1 },
        SDL::Key{ SDLK_1, { SDL::Modifier::CTRL, SDL::Modifier::ALT, SDL::Modifier::SHIFT, SDL::Modifier::GUI } },
        SDL::Key{ SDLK_1, { SDL::Modifier::LCTRL } },
        SDL::Key{ SDLK_1, { SDL::Modifier::RCTRL } },
        SDL::Key{ SDLK_1, { SDL::Modifier::LALT } },
        SDL::Key{ SDLK_1, { SDL::Modifier::RALT } },
        SDL::Key{ SDLK_1, { SDL::Modifier::LGUI } },
        SDL::Key{ SDLK_1, { SDL::Modifier::RGUI } },
    };

    for (const auto& key : keys) {

        const auto keys_string = key.to_string();

        const auto parsed = SDL::Key::from_string(keys_string);


        ASSERT_THAT(parsed, ExpectedHasValue()) << "Input was: " << key;
        ASSERT_EQ(parsed.value(), key);
    }
}
