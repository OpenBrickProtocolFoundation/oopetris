#pragma once

#include "static_string.hpp"

namespace utils {


    template<usize data_size>
    constexpr auto get_supported_music_extension(
            const char( // NOLINT(modernize-avoid-c-arrays,cppcoreguidelines-avoid-c-arrays)
                    &name
            )[data_size]
    ) {

#if not defined(AUDIO_PREFER_MP3) and not defined(AUDIO_PREFER_FLAC)
#define AUDIO_PREFER_FLAC
#elif defined(AUDIO_PREFER_MP3) and defined(AUDIO_PREFER_FLAC)
#error "Can't prefer FLAC and MP3"
#endif

#if defined(AUDIO_WITH_FLAC_SUPPORT) and ((not defined(AUDIO_WITH_MP3_SUPPORT)) or defined(AUDIO_PREFER_FLAC))
        constexpr auto ext = StaticString{ "flac" };
#elif defined(AUDIO_WITH_MP3_SUPPORT) and ((not defined(AUDIO_WITH_FLAC_SUPPORT)) or defined(AUDIO_PREFER_MP3))
        constexpr auto ext = StaticString{ "mp3" };
#else
#error "Either FLAC or MP3 support has to be available at built time"
#endif

        return StaticString{ name } + StaticString{ "." } + ext;
    }

} // namespace utils
