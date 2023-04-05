

#pragma once

#include <cstddef>
#include <cstdint>
#include <utility>

using RawBytes = std::pair<std::uint8_t*, std::uint32_t>;

/* abstract*/ struct Transportable {
public:
    virtual ~Transportable() = default;
    // if you change the protocol behaviour in some way, change this  number
    static constexpr uint32_t protocol_version = 1;
    static constexpr uint32_t checksum_size = sizeof(uint32_t);
    static constexpr uint32_t header_size = sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t);

    //TODO de-serialize
    // static std::map<uint32_t, Transportable> deserialize_map = std::map<uint32_t, Transportable>;

    // every class has to have such a serialUUID in some way
    //TODO enforce this in some compile time way, that they are unique!
    virtual uint32_t serialUUID() const = 0;
    static RawBytes serialize(const Transportable* transportable, std::uint32_t data_size);

protected:
    explicit Transportable() { }
    static uint32_t checksum(RawBytes bytes);

private:
    static void write_header(RawBytes bytes, uint32_t serialUUID, uint32_t data_size);
    static void write_data(RawBytes bytes, const Transportable* transportable);
    static void write_checksum(RawBytes bytes);
};