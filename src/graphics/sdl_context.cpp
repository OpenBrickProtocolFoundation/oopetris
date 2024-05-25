#include <core/helper/errors.hpp>

#include "graphics/sdl_context.hpp"

#include <SDL.h>
#include <SDL_ttf.h>
#include <fmt/format.h>

#if defined(__CONSOLE__)
#include "helper/console_helpers.hpp"
#endif

#if defined(_HAVE_FILE_DIALOGS)

#include "helper/nfd_include.hpp"

#endif

SdlContext::SdlContext() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        throw helper::InitializationError{ fmt::format("Failed in initializing sdl: {}", SDL_GetError()) };
    }

    if (TTF_Init() < 0) {
        throw helper::InitializationError{ fmt::format("Failed in initializing sdl ttf: {}", TTF_GetError()) };
    }

#if defined(__CONSOLE__)
    console::platform_init();
#endif

#if defined(_HAVE_FILE_DIALOGS)
    if (NFD::Init() != NFD_OKAY) {
        throw helper::InitializationError{
            fmt::format("Failed to initialize the file dialog library: {}", NFD::GetError())
        };
    }

#endif
}

SdlContext::~SdlContext() {

#if defined(_HAVE_FILE_DIALOGS)
    NFD::Quit();
#endif

#if defined(__CONSOLE__)
    console::platform_exit();
#endif

    TTF_Quit();
    SDL_Quit();
}
