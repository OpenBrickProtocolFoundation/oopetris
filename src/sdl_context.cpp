#include "sdl_context.hpp"
#include <SDL.h>
#include <SDL_ttf.h>
#include <spdlog/spdlog.h>

#if defined(__SWITCH__)
#include <switch.h>
#endif

#if defined(__3DS__)
#include <3ds.h>
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

    //TODO, since local multiplayer on a switch is possible, test here if there are more then one joysticks available (e.g. left and right controller) then ask the user if he wants to play local multiplayer, implement that in JoystickInput (in the SDL event the joystick index is present)
    SDL_JoystickOpen(0);
#elif defined(__3DS__)

    // based on: https://github.com/devkitPro/3ds-examples

    // mount the romfs in the executable as "romfs:/" (this is fine since only one app can run at the time on the 3ds)
    Result ret = romfsInit();
    if (R_FAILED(ret)) {
        spdlog::error("romfsInit() failed: 0x{:08x}", (unsigned int) ret);
        std::exit(1);
    }

    // init joystick and other nintendo 3ds specific things
    SDL_InitSubSystem(SDL_INIT_JOYSTICK);
    SDL_JoystickEventState(SDL_ENABLE);
    // only use the first joystick (3ds also has only one)

    SDL_JoystickOpen(0);
#endif
}

SdlContext::~SdlContext() {
    TTF_Quit();
    SDL_Quit();

#if defined(__SWITCH__) or defined(__3DS__)
    // unmount romfs again
    romfsExit();
#endif
}
