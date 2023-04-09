#pragma once

#include <bit>
#include <concepts>
#include <string>

namespace utils {
    [[nodiscard]] std::string current_date_time_iso8601();

    [[nodiscard]] constexpr inline auto to_little_endian(std::integral auto value) {
        if constexpr (std::endian::native == std::endian::little) {
            return value;
        } else {
            return std::byteswap(value);
        }
    }

    [[nodiscard]] constexpr inline auto from_little_endian(std::integral auto value) {
        return to_little_endian(value);
    }
} // namespace utils
