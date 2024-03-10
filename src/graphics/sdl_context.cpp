#include "graphics/sdl_context.hpp"

#include <SDL.h>
#include <SDL_ttf.h>
#include <stdexcept>
#include <string>

#if defined(__SWITCH__)
#include "switch.h"
#endif

#if defined(_HAVE_FILE_DIALOGS)

#include "helper/nfd_include.hpp"

#endif

SdlContext::SdlContext() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        throw std::runtime_error{ "failed in initializing sdl: " + std::string{ SDL_GetError() } };
    }

    if (TTF_Init() < 0) {
        throw std::runtime_error{ "failed in initializing sdl ttf: " + std::string{ TTF_GetError() } };
    }

#if defined(__SWITCH__)
    // based on: https://github.com/carstene1ns/switch-sdl2-demo

    // mount the romfs in the executable as "romfs:/" (this is fine since only one app can run at the time on the switch)
    romfsInit();

    // init joystick and other nintendo switch specific things
    SDL_InitSubSystem(SDL_INIT_JOYSTICK);
    SDL_JoystickEventState(SDL_ENABLE);
    // only use the first joystick!

    //TODO, since local multiplayer on a switch is possible, test here if there are more then one joysticks available (e.g. left and right controller) then ask the user if he wants to play local multiplayer, implement that in JoystickInput (in the SDL event the joystick index is present)
    SDL_JoystickOpen(0);

#endif

#if defined(_HAVE_FILE_DIALOGS)
    if (NFD::Init() != NFD_OKAY) {
        throw std::runtime_error{ "failed to initialize the file dialog library: " + std::string{ NFD::GetError() } };
    }

#endif
}

SdlContext::~SdlContext() {

#if defined(_HAVE_FILE_DIALOGS)
    NFD::Quit();
#endif

#if defined(__SWITCH__)
    // unmount romfs again
    romfsExit();
#endif

    TTF_Quit();
    SDL_Quit();
}
