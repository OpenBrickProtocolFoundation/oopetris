
#include "platform.hpp"
#include "helper/utils.hpp"

#if defined(__CONSOLE__)
#include "helper/console_helpers.hpp"
#include "input/console_buttons.hpp"
#endif

#include <SDL.h>
#include <cerrno>
#include <cstdlib>
#include <spdlog/spdlog.h>

namespace {

    inline std::string get_error_from_errno() {

#if defined(_MSC_VER)
        char buffer[256] = { 0 };
        const auto result = strerror_s<256>(buffer, errno);

        if (result == 0) {
            return std::string{ buffer };

        } else {
            return std::string{ "Error while getting error!" };
        }

#else
        return std::string{ std::strerror(errno) };

#endif
    }


} // namespace


[[nodiscard]] std::string utils::built_for_platform() {
#if defined(__ANDROID__)
    return "Android";
#elif defined(__SWITCH__)
    return "Nintendo Switch";
#elif defined(__3DS__)
    return "Nintendo 3DS";
#elif defined(FLATPAK_BUILD)
    return "Linux (Flatpak)";
#elif defined(__linux__)
    return "Linux";
#elif defined(_WIN32)
    return "Windows";
#elif defined(__APPLE__)
    return "MacOS";
#elif defined(__SERENITY__)
    return "Serenity OS";
#else
#error "Unsupported platform"
#endif
}

// partially from: https://stackoverflow.com/questions/17347950/how-do-i-open-a-url-from-c
[[nodiscard]] bool utils::open_url(const std::string& url) {
#if defined(__ANDROID__)
    const auto result = SDL_OpenURL(url.c_str());
    if (result < 0) {
        spdlog::error("Error in opening url in android: {}", SDL_GetError());
        return false;
    }

    return true;

#elif defined(__CONSOLE__)
    auto result = console::open_url(url);
    spdlog::info("Returned string from url open was: {}", result);
    return true;
#elif defined(__SERENITY__)
    UNUSED(url);
    return false;
#else
    //TODO(Totto): this is dangerous, if we supply user input, so use SDL_OpenURL preferably

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    const std::string shell_command = "start " + url;
#elif defined(__APPLE__)
    const std::string shell_command = "open " + url;
#elif defined(__linux__)
    const std::string shell_command = "xdg-open " + url;
#else
#error "Unsupported platform"
#endif

    const auto result = system(shell_command.c_str()); //NOLINT(cert-env33-c)
    if (result < 0) {
        spdlog::error("Error in opening url: {}", get_error_from_errno());
        return false;
    }


    return true;


#endif
}
