#include "sdl_context.hpp"

#include "SDL.h"

SdlContext::SdlContext() {
    SDL_Init(SDL_INIT_VIDEO);
}

SdlContext::~SdlContext() {
    SDL_Quit();
}
