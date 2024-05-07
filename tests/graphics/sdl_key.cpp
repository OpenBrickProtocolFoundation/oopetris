
#include "manager/sdl_key.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>


//TODO: improve

TEST(SDLKey, SimpleComparision) {
    const auto key1 = SDL::Key{ SDLK_k };

    ASSERT_EQ(key1.name(), "K");
    ASSERT_EQ(key1.has_modifier(SDL::Modifier::ALT), false);
}
