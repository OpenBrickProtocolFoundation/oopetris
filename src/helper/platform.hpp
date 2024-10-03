

#pragma once

#include <core/helper/types.hpp>

#include <string>

#include "./windows.hpp"

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


    OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] std::string built_for_platform();

    OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] bool open_url(const std::string& url);

} // namespace utils
