
#include <core/helper/expected.hpp>

#include "manager/sdl_key.hpp"
#include "utils/helper.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <tuple>
#include <vector>


namespace sdl {

    // make keys printable
    void PrintTo(const Key& key, std::ostream* os) {
        *os << key.to_string();
    }


    std::ostream& operator<<(std::ostream& os, const Key& value) {
        os << value.to_string();
        return os;
    }
} // namespace sdl

TEST(SDLKey, SimpleComparision) {
    const auto key1 = sdl::Key{ SDLK_k };

    ASSERT_EQ(key1.to_string(), "K");
    ASSERT_EQ(key1.has_modifier(sdl::Modifier::ALT), false);
}

TEST(SDLKey, FromString) {

    const std::vector<std::tuple<helper::expected<sdl::Key, std::string>, std::string>> strings{
        {                                                               sdl::Key{ SDLK_1, { sdl::Modifier::CTRL } },"Ctrl + 1"                                                                                                                    },
        {                                                             sdl::Key{ SDLK_1, { sdl::Modifier::LSHIFT } },       "Shift-L + 1" },
        {                                                             sdl::Key{ SDLK_1, { sdl::Modifier::RSHIFT } },       "Shift-R + 1" },
        {                                       helper::unexpected<std::string>{ "Not a valid modifier: 'ShiftL'" },        "ShiftL + 1" },
        {                                          helper::unexpected<std::string>{ "Duplicate modifier: 'Shift'" }, "Shift + Shift + 1" },
        {                                      helper::unexpected<std::string>{ "No key but only modifiers given" },             "Shift" },
        {                                                          helper::unexpected<std::string>{ "Empty token" },                  "" },
        {                                                          helper::unexpected<std::string>{ "Empty token" },         "   + \t  " },
        {                                                                                        sdl::Key{ SDLK_1 },                 "1" },
        { sdl::Key{ SDLK_1, { sdl::Modifier::CTRL, sdl::Modifier::ALT, sdl::Modifier::SHIFT, sdl::Modifier::GUI } },
         "Shift + Alt + Ctrl + Gui + 1"                                                                                                 },
        {                                                              sdl::Key{ SDLK_1, { sdl::Modifier::LCTRL } },        "Ctrl-L + 1" },
        {                                                              sdl::Key{ SDLK_1, { sdl::Modifier::RCTRL } },        "Ctrl-R + 1" },
        {                                                               sdl::Key{ SDLK_1, { sdl::Modifier::LALT } },         "Alt-L + 1" },
        {                                                               sdl::Key{ SDLK_1, { sdl::Modifier::RALT } },         "Alt-R + 1" },
        {                                                               sdl::Key{ SDLK_1, { sdl::Modifier::LGUI } },         "Gui-L + 1" },
        {                                                               sdl::Key{ SDLK_1, { sdl::Modifier::RGUI } },         "Gui-R + 1" },
    };

    for (const auto& [correct, str] : strings) {

        const auto parsed = sdl::Key::from_string(str);

        if (correct.has_value()) {
            ASSERT_THAT(parsed, ExpectedHasValue()) << "Input was: " << str;
            ASSERT_EQ(correct.value(), parsed.value());
        } else {
            ASSERT_THAT(parsed, ExpectedHasError()) << "Input was: " << str;
            ASSERT_EQ(correct.error(), parsed.error());
        }
    }
}


TEST(SDLKey, ModifierTests) {
    const auto key1 = sdl::Key{ SDLK_k, { sdl::Modifier::CTRL } };

    ASSERT_EQ(key1.has_modifier(sdl::Modifier::ALT), false);

    ASSERT_EQ(key1.has_modifier(sdl::Modifier::LCTRL), true);
    ASSERT_EQ(key1.has_modifier(sdl::Modifier::RCTRL), true);
    ASSERT_EQ(key1.has_modifier(sdl::Modifier::CTRL), true);

    ASSERT_EQ(key1.has_modifier_exact(sdl::Modifier::LCTRL), false);
    ASSERT_EQ(key1.has_modifier_exact(sdl::Modifier::RCTRL), false);
    ASSERT_EQ(key1.has_modifier_exact(sdl::Modifier::CTRL), true);
}


TEST(SDLKey, SymmetryOfEqual) {

    const std::vector<std::tuple<sdl::Key, sdl::Key, bool>> keys{
        { sdl::Key{ SDLK_ESCAPE, { sdl::Modifier::LCTRL } }, sdl::Key{ SDLK_ESCAPE, { sdl::Modifier::LCTRL } },  true },
        { sdl::Key{ SDLK_ESCAPE, { sdl::Modifier::LCTRL } },  sdl::Key{ SDLK_ESCAPE, { sdl::Modifier::CTRL } },  true },
        { sdl::Key{ SDLK_ESCAPE, { sdl::Modifier::RCTRL } },  sdl::Key{ SDLK_ESCAPE, { sdl::Modifier::CTRL } },  true },
        { sdl::Key{ SDLK_ESCAPE, { sdl::Modifier::RCTRL } }, sdl::Key{ SDLK_ESCAPE, { sdl::Modifier::LCTRL } }, false },
        {  sdl::Key{ SDLK_ESCAPE, { sdl::Modifier::CTRL } },  sdl::Key{ SDLK_ESCAPE, { sdl::Modifier::CTRL } },  true },
        {  sdl::Key{ SDLK_ESCAPE, { sdl::Modifier::CTRL } },                           sdl::Key{ SDLK_ESCAPE }, false },
        {  sdl::Key{ SDLK_ESCAPE, { sdl::Modifier::LALT } },  sdl::Key{ SDLK_ESCAPE, { sdl::Modifier::CTRL } }, false },
        {  sdl::Key{ SDLK_ESCAPE, { sdl::Modifier::RALT } },  sdl::Key{ SDLK_ESCAPE, { sdl::Modifier::CTRL } }, false },
        {  sdl::Key{ SDLK_ESCAPE, { sdl::Modifier::LGUI } },  sdl::Key{ SDLK_ESCAPE, { sdl::Modifier::CTRL } }, false },
    };

    for (const auto& [key, other_key, expected] : keys) {

        const auto are_equal = key == other_key;

        ASSERT_EQ(are_equal, expected) << "wrong result for: " << key << " == " << other_key;

        const auto are_equal_2 = key == other_key;

        ASSERT_EQ(are_equal_2, expected) << "wrong result for: " << other_key << " == " << key;
    }
}


TEST(SDLKey, ToString) {

    const std::vector<sdl::Key> keys{
        sdl::Key{ SDLK_1, { sdl::Modifier::CTRL } },
        sdl::Key{ SDLK_1, { sdl::Modifier::LSHIFT } },
        sdl::Key{ SDLK_1, { sdl::Modifier::RSHIFT } },
        sdl::Key{ SDLK_1 },
        sdl::Key{ SDLK_1, { sdl::Modifier::CTRL, sdl::Modifier::ALT, sdl::Modifier::SHIFT, sdl::Modifier::GUI } },
        sdl::Key{ SDLK_1, { sdl::Modifier::LCTRL } },
        sdl::Key{ SDLK_1, { sdl::Modifier::RCTRL } },
        sdl::Key{ SDLK_1, { sdl::Modifier::LALT } },
        sdl::Key{ SDLK_1, { sdl::Modifier::RALT } },
        sdl::Key{ SDLK_1, { sdl::Modifier::LGUI } },
        sdl::Key{ SDLK_1, { sdl::Modifier::RGUI } },
    };

    for (const auto& key : keys) {

        const auto keys_string = key.to_string();

        const auto parsed = sdl::Key::from_string(keys_string);


        ASSERT_THAT(parsed, ExpectedHasValue()) << "Input was: " << key;
        ASSERT_EQ(parsed.value(), key);
    }
}


TEST(SDLKey, ComplexComparison) {

    const auto to_compare = sdl::Key{ SDLK_ESCAPE, { sdl::Modifier::CTRL } };

    const std::vector<std::tuple<sdl::Key, bool>> keys{
        {                         sdl::Key{ SDLK_1, { sdl::Modifier::LCTRL } }, false },
        {                    sdl::Key{ SDLK_ESCAPE, { sdl::Modifier::LCTRL } },  true },
        {                    sdl::Key{ SDLK_ESCAPE, { sdl::Modifier::RCTRL } },  true },
        {                     sdl::Key{ SDLK_ESCAPE, { sdl::Modifier::CTRL } },  true },
        {                                              sdl::Key{ SDLK_ESCAPE }, false },
        { sdl::Key{ SDLK_ESCAPE, { sdl::Modifier::CTRL, sdl::Modifier::GUI } }, false },
        {                     sdl::Key{ SDLK_ESCAPE, { sdl::Modifier::LALT } }, false },
        {                     sdl::Key{ SDLK_ESCAPE, { sdl::Modifier::RALT } }, false },
        {                     sdl::Key{ SDLK_ESCAPE, { sdl::Modifier::LGUI } }, false },
    };


    for (const auto& [key, expected] : keys) {

        const auto are_equal = to_compare == key;

        ASSERT_EQ(are_equal, expected) << "wrong result for: " << to_compare << " == " << key;
    }
}
