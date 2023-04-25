#pragma once

#include "static_string.hpp"

namespace constants {


    inline constexpr auto program_name = StaticString{ "oopetris" };
    inline constexpr auto author = StaticString{ "coder2k" };
    inline constexpr auto version = StaticString{ "0.0.1" };
    inline constexpr auto music_change_level = 30;

#if not defined(AUDIO_QUALITY)
#define AUDIO_QUALITY 0
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
