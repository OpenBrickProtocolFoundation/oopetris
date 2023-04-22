

#pragma once


// some switch buttons, from https://github.com/carstene1ns/switch-sdl2-demo

//using C enum (not enum class) on purpose
enum JOYCON {

    JOYCON_A = 0,
    JOYCON_B = 1,
    JOYCON_X = 2,
    JOYCON_Y = 3,
    // SDL_CONTROLLER_BUTTON_MISC1 = is plus according to SDL but thats incorrect ?
    JOYCON_PLUS = 10,
    JOYCON_MINUS = 11,
    JOYCON_CROSS_LEFT = 12,
    JOYCON_CROSS_UP = 13,
    JOYCON_CROSS_RIGHT = 14,
    JOYCON_CROSS_DOWN = 15,
    JOYCON_LDPAD_LEFT = 16,
    JOYCON_LDPAD_UP = 17,
    JOYCON_LDPAD_RIGHT = 18,
    JOYCON_LDPAD_DOWN = 19,
    JOYCON_RDPAD_LEFT = 20,
    JOYCON_RDPAD_UP = 21,
    JOYCON_RDPAD_RIGHT = 22,
    JOYCON_RDPAD_DOWN = 23
};

//TODO debug the SDL mapping if it's just wrong or what the problem might be.
// These are the SDL CONSTANTS; but they seem invalid, I tested it and the ones from the example work, the SDl ones not??
//  SDL_CONTROLLER_BUTTON_INVALID = -1,
//     SDL_CONTROLLER_BUTTON_A,
//     SDL_CONTROLLER_BUTTON_B,
//     SDL_CONTROLLER_BUTTON_X,
//     SDL_CONTROLLER_BUTTON_Y,
//     SDL_CONTROLLER_BUTTON_BACK,
//     SDL_CONTROLLER_BUTTON_GUIDE,
//     SDL_CONTROLLER_BUTTON_START,
//     SDL_CONTROLLER_BUTTON_LEFTSTICK,
//     SDL_CONTROLLER_BUTTON_RIGHTSTICK,
//     SDL_CONTROLLER_BUTTON_LEFTSHOULDER,
//     SDL_CONTROLLER_BUTTON_RIGHTSHOULDER,
//     SDL_CONTROLLER_BUTTON_DPAD_UP,
//     SDL_CONTROLLER_BUTTON_DPAD_DOWN,
//     SDL_CONTROLLER_BUTTON_DPAD_LEFT,
//     SDL_CONTROLLER_BUTTON_DPAD_RIGHT,
//     SDL_CONTROLLER_BUTTON_MISC1,    /* Xbox Series X share button, PS5 microphone button, Nintendo Switch Pro capture button, Amazon Luna microphone button */
//     SDL_CONTROLLER_BUTTON_PADDLE1,  /* Xbox Elite paddle P1 */
//     SDL_CONTROLLER_BUTTON_PADDLE2,  /* Xbox Elite paddle P3 */
//     SDL_CONTROLLER_BUTTON_PADDLE3,  /* Xbox Elite paddle P2 */
//     SDL_CONTROLLER_BUTTON_PADDLE4,  /* Xbox Elite paddle P4 */
//     SDL_CONTROLLER_BUTTON_TOUCHPAD, /* PS4/PS5 touchpad button */
//     SDL_CONTROLLER_BUTTON_MAX
