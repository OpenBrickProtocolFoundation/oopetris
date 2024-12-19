#include <core/helper/errors.hpp>

#include "graphics/sdl_context.hpp"

#include <SDL.h>
#include <SDL_ttf.h>
#include <fmt/format.h>
#include <spdlog/spdlog.h>

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


    // when using gdb / lldb to debug and you click something with the mouse, no other application can use the mouse, which is annoying, so not using this feature in debug mode
#if !defined(NDEBUG)
    const auto hint_mouse_result = SDL_SetHint(SDL_HINT_MOUSE_AUTO_CAPTURE, "0");

    if (hint_mouse_result != SDL_TRUE) {
        // this is non fatal, so not returning
        spdlog::error("Failed to set the SDL_HINT_MOUSE_AUTO_CAPTURE hint: {}", SDL_GetError());
    }
#endif


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
