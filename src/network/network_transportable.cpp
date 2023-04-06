

#include "network_transportable.hpp"
#include "../util.hpp"
#include "crc32.h"
#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <tl/expected.hpp>
#include <tuple>


std::uint32_t Transportable::checksum(RawBytes bytes) {

    auto [start, length] = bytes;

    std::uint32_t table[256];
    crc32::generate_table(table);
    std::uint32_t CRC = 0;
    for (std::uint32_t i = 0; i < length; ++i) {
        CRC = crc32::update(table, CRC, start, 1);
        start++;
    }

    return CRC;
}


void Transportable::write_header(RawBytes bytes, std::uint32_t serialUUID, std::uint32_t data_size) {
    auto [start, length] = bytes;

    //TODO remove assert
    assert(length == Transportable::header_size);

    std::uint32_t* data_ptr = (std::uint32_t*) start;

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

    std::uint32_t data_size = length - Transportable::checksum_size;

    std::uint32_t checksum = Transportable::checksum(RawBytes{ start, data_size });

    std::uint32_t* data_ptr = (std::uint32_t*) ((uint8_t*) start + data_size);

    data_ptr[0] = checksum;
}

RawTransportData::RawTransportData(std::uint32_t serialUUID, RawBytes data) : m_serialUUID{ serialUUID }, m_data{ data } {};


MaybeRawTransportData RawTransportData::from_raw_bytes(RawBytes raw_bytes) {

    auto result = std::vector<std::unique_ptr<RawTransportData>>{};

    auto [start, length] = raw_bytes;
    long remaining_length = length;
    auto advance = [&](std::uint32_t size) {
        remaining_length -= size;
        start += size;
    };
    while (remaining_length > 0) {

        auto header = RawTransportData::read_header(RawBytes{ start, remaining_length });
        if (!header.has_value()) {
            return tl::make_unexpected("in RawTransportData::from_raw_bytes: " + header.error());
        }

        advance(Transportable::header_size);

        auto [_protocol_version, serialUUID, data_size] = header.value();

        if (remaining_length < (long) data_size) {
            return tl::make_unexpected(
                    "in RawTransportData::from_raw_bytes: couldn't read data, since the raw data is to small"
            );
        }

        //TODO check if implemented correctly
        // this malloc get'S freed in the unique ptr destructor later
        void* memory = std::malloc(data_size);
        if (!memory) {
            return tl::make_unexpected("in RawTransportData::from_raw_bytes: error in malloc for RawTransportData");
        }
        auto data = RawBytes{ (uint8_t*) std::memcpy(memory, start, data_size), data_size };

        auto checksum = RawTransportData::read_checksum(RawBytes{ start, remaining_length }, data_size);

        advance(data_size + Transportable::checksum_size);
        result.push_back(std::make_unique<RawTransportData>(serialUUID, data));
    }

    return result;
}

tl::expected<std::tuple<std::uint32_t, std::uint32_t, std::uint32_t>, std::string> RawTransportData::read_header(
        RawBytes bytes
) {
    auto [start, length] = bytes;
    if (length < Transportable::header_size) {
        return tl::make_unexpected("couldn't read header, since the raw data is to small");
    }

    std::uint32_t* data_ptr = (std::uint32_t*) start;

    std::uint32_t protocol_version_number = data_ptr[0];
    if (RawTransportData::protocol_version != protocol_version_number) {
        return tl::make_unexpected(
                "couldn't parse header, since the protocol version mismatches: parser can parse: "
                + std::to_string(RawTransportData::protocol_version)
                + "but received: " + std::to_string(protocol_version_number)
        );
    }

    std::uint32_t serialUUID = data_ptr[1];
    std::uint32_t data_size = data_ptr[2];
    return std::tuple{ protocol_version_number, serialUUID, data_size };
}


tl::expected<std::uint32_t, std::string> RawTransportData::read_checksum(RawBytes bytes, std::uint32_t data_size) {
    auto [start, length] = bytes;
    if (length < data_size + Transportable::checksum_size) {
        return tl::make_unexpected("couldn't read checksum, since the raw data is to small");
    }

    std::uint32_t calc_checksum = Transportable::checksum(RawBytes{ start, data_size });

    std::uint32_t* data_ptr = (std::uint32_t*) ((uint8_t*) start + data_size);

    std::uint32_t read_checksum = data_ptr[0];

    if (read_checksum != calc_checksum) {
        return tl::make_unexpected(
                "couldn't read data, since the checksum mismatches: read checksum: " + util::to_hex_str(read_checksum)
                + "but calculated checksum: " + util::to_hex_str(calc_checksum)
        );
    }

    return read_checksum;
}