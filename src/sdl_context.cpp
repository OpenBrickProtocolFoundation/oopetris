#include "sdl_context.hpp"
#include <SDL.h>
#include <SDL_ttf.h>

SdlContext::SdlContext() {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
}

SdlContext::~SdlContext() {
    TTF_Quit();
    SDL_Quit();
}
