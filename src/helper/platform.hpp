

#pragma once

#include <core/helper/types.hpp>

#include <string>

#include "./export_symbols.hpp"

enum class Platform : u8 { PC, Android, Console, Web };


namespace utils {

    constexpr Platform get_platform() {

#if defined(__ANDROID__)
        return Platform::Android;
#elif defined(__CONSOLE__)
        return Platform::Console;
#elif defined(__EMSCRIPTEN__)
        return Platform::Web;
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


    [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED std::string built_for_platform();

    [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED bool open_url(const std::string& url);

} // namespace utils
