
#include "helper/errors.hpp"
#if defined(__NINTENDO_CONSOLE__)

#include "console_helpers.hpp"

#if defined(__SWITCH__)
#include <switch.h>
#elif defined(__3DS__)
#include <3ds.h>
#endif

void console::debug_write(const char* text, size_t size) {
#ifdef __3DS__
    svcOutputDebugString(text, size);
#elif defined(__SWITCH__)
    svcOutputDebugString(text, size);
#else
#error "not implemented"
#endif
}

void console::platform_init() {
#if defined(__3DS__) || defined(__SWITCH__)
    Result ret = romfsInit();
    if (R_FAILED(ret)) {
        throw helper::IniInitializationError(fmt::format("romfsInit() failed: {:#2x}", (unsigned int) ret));
    }
#else
#error "not implemented"
#endif
}

void console::platform_exit() {
#if defined(__3DS__) || defined(__SWITCH__)
    Result ret = romfsExit();
    if (R_FAILED(ret)) {
        throw helper::IniInitializationError(fmt::format("romfsExit() failed: {:#2x}", (unsigned int) ret));
    }
#else
#error "not implemented"
#endif
}

bool console::inMainLoop() {
#ifdef __3DS__
    return aptMainLoop();
#elif defined(__SWITCH__)
    return appletMainLoop();
#else
#error "not implemented"
#endif
}


#endif
