#pragma once

#include "utils.hpp"
#include <algorithm>
#include <array>
#include <bit>
#include <concepts>
#if defined(__ANDROID__)
#include "bit.hpp"
#include "concepts.hpp"
#endif

#if not defined(__ANDROID__)
#include <ranges>
#else
#include <algorithm>
#endif
#include <string>

namespace utils {
    [[nodiscard]] std::string current_date_time_iso8601();

    template<std::integral Integral>
    [[nodiscard]] constexpr inline Integral byte_swap(Integral value) noexcept {
        // source: https://en.cppreference.com/w/cpp/numeric/byteswap
        static_assert(std::has_unique_object_representations_v<Integral>, "T may not have padding bits");
        auto value_representation = std::bit_cast<std::array<std::byte, sizeof(Integral)>>(value);
#if not defined(__ANDROID__)
        std::ranges::reverse(value_representation);
#else
        std::reverse(value_representation);
#endif

        return std::bit_cast<Integral>(value_representation);
    }

    [[nodiscard]] constexpr inline auto to_little_endian(std::integral auto value) {
        if constexpr (std::endian::native == std::endian::little) {
            return value;
        } else {
            return byte_swap(value);
        }
    }

    [[nodiscard]] constexpr inline auto from_little_endian(std::integral auto value) {
        return to_little_endian(value);
    }
} // namespace utils
