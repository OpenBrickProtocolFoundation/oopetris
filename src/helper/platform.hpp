

#pragma once

#include "helper/types.hpp"

#include <string>

enum class Platform : u8 { PC, Android, Console };


namespace utils {

    constexpr Platform get_platform() {

#if defined(__ANDROID__)
        return Platform::Android;
#elif defined(__CONSOLE__)
        return Platform::Console;
#else
        return Platform::PC;
#endif
    };


    enum class Orientation : u8 {
        Portrait, // 9x16, e.g. smartphone
        Landscape //  16x9
    };


    constexpr Orientation get_orientation() {
#if defined(__ANDROID__)
        return Orientation::Portrait;
#else
        return Orientation::Landscape;
#endif
    }


    [[nodiscard]] std::string built_for_platform();

    [[nodiscard]] bool open_url(const std::string& url);

} // namespace utils
