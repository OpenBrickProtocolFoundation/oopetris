#pragma once

#include "types.hpp"
#include "utils.hpp"
#include <algorithm>
#include <array>
#include <bit>
#include <climits>
#include <filesystem>
#include <string>
#include <string_view>
#include <tl/optional.hpp>
#include <type_traits>

#if not defined(AUDIO_PREFER_MP3) and not defined(AUDIO_PREFER_FLAC)
#define AUDIO_PREFER_FLAC
#elif defined(AUDIO_PREFER_MP3) and defined(AUDIO_PREFER_FLAC)
#error "Can't prefer FLAC and MP3"
#endif


namespace utils {
    // taken from llvm: https://github.com/llvm/llvm-project/blob/main/libcxx/include/__concepts/arithmetic.h#L27-L30
    // [concepts.arithmetic], arithmetic concepts
    template<class T>
    concept integral = std::is_integral_v<T>;

    [[nodiscard]] std::string current_date_time_iso8601();

    template<integral Integral>
    [[nodiscard]] constexpr Integral byte_swap(Integral value) noexcept {
        // based on source: slartibartswift
        auto result = Integral{};
        for (usize i = 0; i < sizeof(Integral); ++i) {
            result <<= CHAR_BIT;
            result |= value & 0xFF;
            value >>= CHAR_BIT;
        }
        return result;
    }

    [[nodiscard]] constexpr auto to_little_endian(integral auto value) {
        if constexpr (std::endian::native == std::endian::little) {
            return value;
        } else {
            return byte_swap(value);
        }
    }

    [[nodiscard]] constexpr auto from_little_endian(integral auto value) {
        return to_little_endian(value);
    }

    template<class Enum>
    [[nodiscard]] constexpr std::underlying_type_t<Enum> to_underlying(Enum e) noexcept {
        return static_cast<std::underlying_type_t<Enum>>(e);
    }

    [[nodiscard]] std::filesystem::path get_assets_folder();

    [[nodiscard]] std::filesystem::path get_root_folder();

    [[nodiscard]] std::filesystem::path get_subfolder_to_root(std::string_view folder);

    tl::optional<bool> log_error(const std::string& error);


#if defined(AUDIO_WITH_FLAC_SUPPORT) and ((not defined(AUDIO_WITH_MP3_SUPPORT)) or defined(AUDIO_PREFER_FLAC))
#define get_supported_music_extension(X) X ".flac"
#elif defined(AUDIO_WITH_MP3_SUPPORT) and ((not defined(AUDIO_WITH_FLAC_SUPPORT)) or defined(AUDIO_PREFER_MP3))
#define get_supported_music_extension(X) X ".mp3"
#else
#error "Either FLAC or MP3 support has to be available at built time"
#endif


} // namespace utils
