
#pragma once

#include "color.hpp"
#include <SDL.h>


namespace utils {

    [[nodiscard]] SDL_Color sdl_color_from_color(const Color& color);

}
