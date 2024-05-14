#include "graphics/sdl_context.hpp"
#include "helper/errors.hpp"
#include "helper/format.hpp"

#include <SDL.h>
#include <SDL_ttf.h>

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

    //TODO: factor out

    // init joystick and other nintendo switch / 3ds specific things
    SDL_InitSubSystem(SDL_INIT_JOYSTICK);
    SDL_JoystickEventState(SDL_ENABLE);
    // only use the first joystick!

    //TODO, since local multiplayer on a switch / 3ds is possible, test here if there are more then one joysticks available (e.g. left and right controller) then ask the user if he wants to play local multiplayer, implement that in JoystickInput (in the SDL event the joystick index is present)
    SDL_JoystickOpen(0);

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
