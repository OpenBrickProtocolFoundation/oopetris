


#pragma once

#ifdef _USE_SDL_LEGACY_VERSION


#define SDL_KeyCode SDLKey

#define SDLK_KP_0 SDLK_KP0
#define SDLK_KP_1 SDLK_KP1
#define SDLK_KP_2 SDLK_KP2
#define SDLK_KP_3 SDLK_KP3
#define SDLK_KP_4 SDLK_KP4
#define SDLK_KP_5 SDLK_KP5
#define SDLK_KP_6 SDLK_KP6
#define SDLK_KP_7 SDLK_KP7
#define SDLK_KP_8 SDLK_KP8
#define SDLK_KP_9 SDLK_KP9


#define SDLK_PRINTSCREEN SDLK_PRINT
#define SDLK_SCROLLLOCK SDLK_SCROLLOCK
#define SDLK_NUMLOCKCLEAR SDLK_NUMLOCK

#ifdef __3DS__
// see: https://github.com/nop90/SDL-3DS
#define SDL_WINDOWPOS_CENTERED SDL_FULLSCREEN
#define SDL_WINDOWPOS_UNDEFINED SDL_FULLSCREEN

#define SDL_int Sint16
#define SDL_Rect_uint Uint16

#endif

#ifdef _HAVE_SDL_GFX

#include <SDL_gfxPrimitives.h>
#endif

#else

#define SDL_int int
#define SDL_Rect_uint int

#endif
