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
} // namespace utils
