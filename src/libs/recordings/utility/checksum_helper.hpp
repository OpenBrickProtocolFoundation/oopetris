

#pragma once

#include "./export_symbols.hpp"
#include <core/hash-library/sha256.h>
#include <core/helper/utils.hpp>

#include <array>
#include <string>
#include <vector>

struct Sha256Stream {
    static constexpr auto ChecksumSize = hash_library::SHA256::HashBytes;
    using Checksum = std::array<unsigned char, ChecksumSize>;

    hash_library::SHA256 library_object;

    Sha256Stream();

    template<std::integral Integral>
    Sha256Stream& operator<<(const Integral value) {

        library_object.add(
                reinterpret_cast<const void*>(&value), // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
                static_cast<usize>(sizeof(value))
        );
        return *this;
    }

    OOPETRIS_RECORDINGS_EXPORTED Sha256Stream& operator<<(const std::string& value);

    template<typename T>
    Sha256Stream& operator<<(const std::vector<T>& values) {

        for (const auto& value : values) {
            *this << value;
        }
        return *this;
    }

    template<typename T, std::size_t S>
    Sha256Stream& operator<<(const std::array<T, S>& values) {

        for (const auto& value : values) {
            *this << value;
        }
        return *this;
    }

    [[nodiscard]] OOPETRIS_RECORDINGS_EXPORTED Checksum get_hash();
};
