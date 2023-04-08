#include "sdl_context.hpp"
#include "network/network_util.hpp"
#include <SDL.h>
#include <SDL_net.h>
#include <SDL_ttf.h>

SdlContext::SdlContext() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init: %s\n", SDL_GetError());
        exit(2);
    }
    if (TTF_Init() != 0) {
        printf("TTF_Init: %s\n", SDL_GetError());
        exit(2);
    }
    //TODO: if we don't need the network, this should be disabled
    if (SDLNet_Init() == -1) {
        printf("SDLNet_Init: %s\n", network_util::latest_sdl_net_error().c_str());
        exit(2);
    }
}

SdlContext::~SdlContext() {
    SDLNet_Quit();
    TTF_Quit();
    SDL_Quit();
}
