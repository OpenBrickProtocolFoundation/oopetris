#include "graphics/sdl_context.hpp"
#include <SDL.h>
#include <SDL_ttf.h>

#if defined(__SWITCH__)
#include "switch.h"
#endif

SdlContext::SdlContext() {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

#if defined(__SWITCH__)
    // based on: https://github.com/carstene1ns/switch-sdl2-demo

    // mount the romfs in the executable as "romfs:/" (this is fine since only one app can run at the time on the switch)
    romfsInit();

    // init joystick and other nintendo switch specific things
    SDL_InitSubSystem(SDL_INIT_JOYSTICK);
    SDL_JoystickEventState(SDL_ENABLE);
    // only use the first joystick!

    //TODO, since local multiplayer on a switch is possible, test here if there are more then one joysticks available (e.g. left and right controller) then ask the user if he wants to play local multiplayer, implement that in JoystickInput (in the SDL event the joysrick index is present)
    SDL_JoystickOpen(0);

#endif
}

SdlContext::~SdlContext() {
    TTF_Quit();
    SDL_Quit();

#if defined(__SWITCH__)
    // unmount romfs again
    romfsExit();
#endif
}
