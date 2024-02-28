

#pragma once

#include "helper/utils.hpp"

#include <sha256.h>

struct Sha256Stream {
    static constexpr auto ChecksumSize = SHA256::HashBytes;
    using Checksum = std::array<unsigned char, ChecksumSize>;

    SHA256 library_object;

    Sha256Stream() : library_object{} {};

    template<utils::integral Integral>
    Sha256Stream& operator<<(Integral value) {

        library_object.add(reinterpret_cast<void*>(&value), static_cast<usize>(sizeof(value)));
        return *this;
    }

    [[nodiscard]] Checksum get_hash() {
        Checksum buffer{};

        library_object.getHash(buffer.data());

        return buffer;
    }
};
