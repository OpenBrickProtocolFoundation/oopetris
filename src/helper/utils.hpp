#pragma once

#include "helper/optional.hpp"
#include "helper/static_string.hpp"
#include "helper/types.hpp"

#include <algorithm>
#include <array>
#include <bit>
#include <cassert>
#include <climits>
#include <filesystem>
#include <string>
#include <string_view>
#include <type_traits>


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
            result |= value & static_cast<Integral>(std::numeric_limits<unsigned char>::max());
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
    [[nodiscard]] constexpr std::underlying_type_t<Enum> to_underlying(Enum enum_) noexcept {
        return static_cast<std::underlying_type_t<Enum>>(enum_);
    }

    [[nodiscard]] std::filesystem::path get_assets_folder();

    [[nodiscard]] std::filesystem::path get_root_folder();

    helpers::optional<bool> log_error(const std::string& error);

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

    [[noreturn]] inline void unreachable() {
        assert(false and "unreachable");
        // TODO: throw exception in android build
        std::terminate();
    }

} // namespace utils


#define UNUSED(x) (void(x)) // NOLINT(cppcoreguidelines-macro-usage)
