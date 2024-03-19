

#pragma once

#include "helper/utils.hpp"

#include <array>
#include <sha256.h>
#include <string>
#include <vector>

struct Sha256Stream {
    static constexpr auto ChecksumSize = hash_library::SHA256::HashBytes;
    using Checksum = std::array<unsigned char, ChecksumSize>;

    hash_library::SHA256 library_object;

    Sha256Stream();

    template<utils::integral Integral>
    Sha256Stream& operator<<(const Integral value) {

        library_object.add(
                reinterpret_cast<const void*>(&value), // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
                static_cast<usize>(sizeof(value))
        );
        return *this;
    }

    Sha256Stream& operator<<(const std::string& value);

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

    [[nodiscard]] Checksum get_hash();
};
