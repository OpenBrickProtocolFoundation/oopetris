
#include "graphic_utils.hpp"

SDL_Color utils::sdl_color_from_color(const Color& color) {
    return SDL_Color{ color.r, color.g, color.b, color.a };
}
