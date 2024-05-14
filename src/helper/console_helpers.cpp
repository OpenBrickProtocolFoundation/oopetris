
#include "helper/errors.hpp"
#if defined(__NINTENDO_CONSOLE__)

#include "console_helpers.hpp"

#include "helper/utils.hpp"

#if defined(__SWITCH__)
#include <switch.h>
#elif defined(__3DS__)
#include <3ds.h>
#include <cstdlib>
#endif

#include "helper/format.hpp"

void console::debug_write(const char* text, size_t size) {
#ifdef __3DS__
    svcOutputDebugString(text, size);
#elif defined(__SWITCH__)
    svcOutputDebugString(text, size);
#else
#error "not implemented"
#endif
}

#define IN_OUT_BUF_SIZE 0x100

[[nodiscard]] std::string console::open_url(const std::string& url) {
#if defined(__3DS__)
    const auto size = url.size() >= 0x100 ? url.size() + 1 : 0x100;

    char* buffer = reinterpret_cast<char*>(malloc(size));

    memcpy(buffer, url.c_str(), url.size());
    buffer[url.size()] = '\0';

    aptLaunchLibraryApplet(APPID_WEB, buffer, size, 0);

    const auto result = std::string{ buffer };

    free(buffer);

    return result;
#elif defined(__SWITCH__)
    UNUSED(url);
    return "";
#else
#error "not implemented"
#endif
}


void console::platform_init() {
#if defined(__3DS__) || defined(__SWITCH__)
    Result ret = romfsInit();
    if (R_FAILED(ret)) {
        throw helper::InitializationError(fmt::format("romfsInit() failed: {:#2x}", static_cast<unsigned int>(ret)));
    }
#else
#error "not implemented"
#endif
}

void console::platform_exit() {
#if defined(__3DS__) || defined(__SWITCH__)
    Result ret = romfsExit();
    if (R_FAILED(ret)) {
        throw helper::InitializationError(fmt::format("romfsExit() failed: {:#2x}", static_cast<unsigned int>(ret)));
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
