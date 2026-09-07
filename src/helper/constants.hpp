#pragma once

#include <core/helper/static_string.hpp>

namespace constants {

#define STRINGIFY(a) STRINGIFY_HELPER_(a) //NOLINT(cppcoreguidelines-macro-usage)
#define STRINGIFY_HELPER_(a) #a           //NOLINT(cppcoreguidelines-macro-usage)

#if !defined(OOPETRIS_NAME) || !defined(OOPETRIS_AUTHOR) || !defined(OOPETRIS_VERSION)
#error "not all needed OOPETRIS_* macros defined"
#endif


    constexpr auto program_name = StaticString{ STRINGIFY(OOPETRIS_NAME) };
    constexpr auto author = StaticString{ STRINGIFY(OOPETRIS_AUTHOR) };
    constexpr auto version = StaticString{ STRINGIFY(OOPETRIS_VERSION) };
    constexpr u32 music_change_level = 30;
    constexpr auto recordings_directory = "recordings";
    constexpr u32 simulation_frequency = 60;

#undef STRINGIFY
#undef STRINGIFY_HELPER_

#if not defined(AUDIO_QUALITY)
#define AUDIO_QUALITY 0 // NOLINT(cppcoreguidelines-macro-usage)
#endif

#if AUDIO_QUALITY == 0 // 0 = HIGH
    inline constexpr auto audio_frequency = 48000;
    inline constexpr auto audio_chunk_size = 1024 * 4;
#elif AUDIO_QUALITY == 1 // 1 = FAST
    inline constexpr auto audio_frequency = 48000;
    inline constexpr auto audio_chunk_size = 1024;
#elif AUDIO_QUALITY == 2 // 0 = LOW
    inline constexpr auto audio_frequency = 44100;
    inline constexpr auto audio_chunk_size = 1024 * 2;
#else
#error "Unrecognized 'AUDIO_QUALITY' value"
#endif


} // namespace constants
