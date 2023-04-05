

#include "network_transportable.hpp"
#include "crc32.h"
#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <stdexcept>


RawBytes Transportable::serialize(const Transportable* transportable, uint32_t data_size) {

    const uint32_t send_size = Transportable::header_size + data_size + Transportable::checksum_size;

    uint8_t* memory = (uint8_t*) std::malloc(send_size);
    if (!memory) {
        throw std::runtime_error{ "error in malloc for sending a message" };
    }


    Transportable::write_header(RawBytes{ memory, Transportable::header_size }, transportable->serialUUID(), data_size);

    Transportable::write_data(RawBytes{ memory + Transportable::header_size, data_size }, transportable);
    Transportable::write_checksum(RawBytes{ memory + Transportable::header_size,
                                            data_size + Transportable::checksum_size });

    return RawBytes{ memory, send_size };
}


uint32_t Transportable::checksum(RawBytes bytes) {

    auto [start, length] = bytes;

    uint32_t table[256];
    crc32::generate_table(table);
    uint32_t CRC = 0;
    for (std::uint32_t i = 0; i < length; ++i) {
        CRC = crc32::update(table, CRC, start, 1);
        start++;
    }

    return CRC;
}


void Transportable::write_header(RawBytes bytes, uint32_t serialUUID, uint32_t data_size) {
    auto [start, length] = bytes;

    assert(length == Transportable::header_size);

    uint32_t* data_ptr = (uint32_t*) start;

    data_ptr[0] = Transportable::protocol_version;

    data_ptr[1] = serialUUID;
    data_ptr[2] = data_size;
}

void Transportable::write_data(RawBytes bytes, const Transportable* transportable) {

    auto [start, length] = bytes;

    uint8_t* data_ptr = (uint8_t*) transportable;
    std::memcpy(start, data_ptr, length);
}


void Transportable::write_checksum(RawBytes bytes) {

    auto [start, length] = bytes;

    uint32_t data_size = length - Transportable::checksum_size;

    uint32_t checksum = Transportable::checksum(RawBytes{ start, data_size });

    uint32_t* data_ptr = (uint32_t*) ((uint8_t*) start + data_size);

    data_ptr[0] = checksum;
}