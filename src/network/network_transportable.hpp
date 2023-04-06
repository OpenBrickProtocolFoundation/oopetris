

#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <sstream>
#include <string>
#include <tl/expected.hpp>
#include <tuple>
#include <utility>
#include <vector>

using RawBytes = std::pair<std::uint8_t*, std::uint32_t>;

/* abstract*/ struct Transportable {
public:
    virtual ~Transportable() = default;
    // if you change the protocol behaviour in some way, change this number and the number in RawTransportData
    static constexpr uint32_t protocol_version = 1;
    static constexpr uint32_t checksum_size = sizeof(uint32_t);
    static constexpr uint32_t header_size = sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t);

    //TODO de-serialize
    // static std::map<uint32_t, Transportable> deserialize_map = std::map<uint32_t, Transportable>;

    // every class has to have such a serialUUID in some way
    //TODO enforce this in some compile time way, that they are unique!
    virtual uint32_t serialUUID() const = 0;
    static RawBytes serialize(const Transportable* transportable, std::uint32_t data_size);

    static uint32_t checksum(RawBytes bytes);

protected:
    explicit Transportable() { }

private:
    static void write_header(RawBytes bytes, uint32_t serialUUID, uint32_t data_size);
    static void write_data(RawBytes bytes, const Transportable* transportable);
    static void write_checksum(RawBytes bytes);
};

//TODO fix inconsistencies in std::size_t and std::uint32_t usage


struct RawTransportData {
private:
    uint32_t m_serialUUID;
    RawBytes m_data;

public:
    RawTransportData(uint32_t serialUUID, RawBytes data);

    // if you change the protocol behaviour in some way, change this number and the number in Transportable (this parses only packets with the same protocol_version)
    static constexpr uint32_t protocol_version = 1;

    static tl::expected<std::vector<std::unique_ptr<RawTransportData>>, std::string> from_raw_bytes(RawBytes raw_bytes);
    static tl::expected<std::tuple<std::uint32_t, std::uint32_t, std::uint32_t>, std::string> read_header(RawBytes bytes
    );
    static tl::expected<std::uint32_t, std::string> read_checksum(RawBytes bytes, uint32_t data_size);

    bool is_of_type(Transportable* transportable);
};

using MaybeRawTransportData = tl::expected<std::vector<std::unique_ptr<RawTransportData>>, std::string>;


template<typename T>
std::string to_hex_str(T number) {
    std::ostringstream ss{};
    ss << std::hex << number;
    return ss.str();
}

template<class T>
using RawUniqueTransportData = std::unique_ptr<T, std::function<void(T*)>>;

// TODO: RawTransportData data is invalid after this, that has to be enforced in some way (at compile time)
template<class T>
RawUniqueTransportData<T> raw_transport_data_to(RawTransportData& data) {
    //TODO pre-check this, after making serialUUID() static!
    //if(RawTransportData::is_of_type(nullptr))
    // TODO: check if this is correct (with free. etc)!
    RawUniqueTransportData<T> raw_unique_ptr{ std::move(data.m_data) /*.first*/, [](void* ptr) { free(ptr); } };
    return raw_unique_ptr;
}
