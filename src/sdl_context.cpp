#include "sdl_context.hpp"

#ifndef __SDL2__FOLDER_MISSING__
#include "SDL2/SDL.h"
#elif
#include "SDL.h"
#endif

SdlContext::SdlContext() {
    SDL_Init(SDL_INIT_VIDEO);
}

SdlContext::~SdlContext() {
    SDL_Quit();
}
