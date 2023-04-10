#pragma once

#include "types.hpp"
#include "utils.hpp"
#include <algorithm>
#include <array>
#include <bit>
#include <climits>
#include <string>
#include <type_traits>

namespace utils {
    // taken from llvm: https://github.com/llvm/llvm-project/blob/main/libcxx/include/__concepts/arithmetic.h#L27-L30
    // [concepts.arithmetic], arithmetic concepts
    template<class T>
    concept integral = std::is_integral_v<T>;

    /**
     * @brief Returns the current date and time as a string in ISO 8601 format without using any separators except for
     * 'T' between the date and the time.
     * @return The current date and time string.
     */
    [[nodiscard]] std::string current_date_time_iso8601();

    /**
     * @brief Converts between big endian and little endian. This function is needed since not all compilers support
     * std::byteswap as of yet.
     * @tparam Integral The type of the value to convert.
     * @param value The value to convert.
     * @return The converted value.
     */
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

    /**
     * @brief Converts from the machine's native byte order to little endian. On a little endian machine, this function
     * does return the input value.
     * @param value A value in the machine's native byte order.
     * @return The value in little endian.
     */
    [[nodiscard]] constexpr auto to_little_endian(integral auto value) {
        if constexpr (std::endian::native == std::endian::little) {
            return value;
        } else {
            return byte_swap(value);
        }
    }

    /**
     * @brief Converts from little endian to the machine's native byte order. On a little endian machine, this function
     * does return the input value.
     * @param value A value in little endian.
     * @return The value in the machine's native byte order.
     */
    [[nodiscard]] constexpr auto from_little_endian(integral auto value) {
        return to_little_endian(value);
    }

    template<class Enum>
    [[nodiscard]] constexpr std::underlying_type_t<Enum> to_underlying(Enum e) noexcept {
        return static_cast<std::underlying_type_t<Enum>>(e);
    }
} // namespace utils
