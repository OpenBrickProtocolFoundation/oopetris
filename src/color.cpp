
#include "color.hpp"


Uint32 Color::to_sdl(const SDL_Surface* surface) const {

    return SDL_MapRGBA(surface->format, r, g, b, a);
}
