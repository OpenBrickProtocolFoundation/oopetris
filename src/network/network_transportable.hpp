

#pragma once

#include <cstddef>
#include <cstdint>
#include <utility>

using RawBytes = std::pair<void*, std::size_t>;

/* abstract*/ class Transportable {
public:
    // every class has to have such a serialUUID in some way
    //TODO enforce this in some compile time way, that they are unique!
    size_t serialUUID = 0;
    static RawBytes serialize(const Transportable transportable);

protected:
    explicit Transportable() { }
    static uint32_t checksum(RawBytes bytes);
};