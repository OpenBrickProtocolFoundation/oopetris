

#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <tl/expected.hpp>
#include <tuple>
#include <typeinfo>
#include <utility>
#include <vector>

using RawBytes = std::pair<std::uint8_t*, std::uint32_t>;

/* abstract*/ struct Transportable {
public:
    virtual ~Transportable() = default;
    // if you change the protocol behaviour in some way, change this number and the number in RawTransportData
    static constexpr std::uint32_t protocol_version = 1;
    static constexpr std::uint32_t checksum_size = sizeof(std::uint32_t);
    static constexpr std::uint32_t header_size = sizeof(std::uint32_t) + sizeof(std::uint32_t) + sizeof(std::uint32_t);

    // every class has to have such a serialUUID in some way
    //TODO enforce this in some compile time way, that they are unique!
    /* virtual */ static constexpr std::uint32_t serialUUID = 0;
    //TODO fix inconsistency and don't raise exceptions, rather return tl:expected
    template<class T>
    static RawBytes serialize(const T* transportable, std::uint32_t data_size) {

        const std::uint32_t send_size = Transportable::header_size + data_size + Transportable::checksum_size;

        uint8_t* memory = (uint8_t*) std::malloc(send_size);
        if (!memory) {
            throw std::runtime_error{ "error in malloc for sending a message" };
        }


        Transportable::write_header(RawBytes{ memory, Transportable::header_size }, T::serialUUID, data_size);

        Transportable::write_data(RawBytes{ memory + Transportable::header_size, data_size }, transportable);
        Transportable::write_checksum(RawBytes{ memory + Transportable::header_size,
                                                data_size + Transportable::checksum_size });

        return RawBytes{ memory, send_size };
    }

    static std::uint32_t checksum(RawBytes bytes);

protected:
    explicit Transportable() { }

private:
    static void write_header(RawBytes bytes, std::uint32_t serialUUID, std::uint32_t data_size);
    static void write_data(RawBytes bytes, const Transportable* transportable);
    static void write_checksum(RawBytes bytes);
};

//TODO fix inconsistencies in std::size_t and std::uint32_t usage


struct RawTransportData {
private:
    std::uint32_t m_serialUUID;
    RawBytes m_data;

public:
    RawTransportData(std::uint32_t serialUUID, RawBytes data);

    // if you change the protocol behaviour in some way, change this number and the number in Transportable (this parses only packets with the same protocol_version)
    static constexpr std::uint32_t protocol_version = 1;

    static tl::expected<std::vector<std::unique_ptr<RawTransportData>>, std::string> from_raw_bytes(RawBytes raw_bytes);
    static tl::expected<std::tuple<std::uint32_t, std::uint32_t, std::uint32_t>, std::string> read_header(RawBytes bytes
    );
    static tl::expected<std::uint32_t, std::string> read_checksum(RawBytes bytes, std::uint32_t data_size);

    bool is_of_type(Transportable* transportable);
};

using MaybeRawTransportData = tl::expected<std::vector<std::unique_ptr<RawTransportData>>, std::string>;


template<class T>
using RawUniqueTransportData = std::unique_ptr<T, std::function<void(T*)>>;


template<class T>
bool raw_transport_data_is_of_type(RawTransportData& data) {
    return data.m_serialUUID == T::serialUUID;
}

template<class T, class P>
bool ptr_raw_transport_data_is_of_type(P data) {
    return data->m_serialUUID == T::serialUUID;
}

// TODO: RawTransportData data is invalid after this, that has to be enforced in some way (at compile time)
template<class T>
RawUniqueTransportData<T> raw_transport_data_to(RawTransportData& data) {
    if (!raw_transport_data_is_of_type<T>(data)) {
        throw std::bad_cast{};
    }
    // TODO: check if this is correct (with free. etc)!
    RawUniqueTransportData<T> raw_unique_ptr{ std::move(data.m_data) /*.first*/, [](void* ptr) { free(ptr); } };
    return raw_unique_ptr;
}

// TODO: RawTransportData data is invalid after this, that has to be enforced in some way (at compile time)
template<class T, class R>
RawUniqueTransportData<T> ptr_raw_transport_data_to(R data) {
    if (!ptr_raw_transport_data_is_of_type<T>(data)) {
        throw std::bad_cast{};
    }
    // TODO: check if this is correct (with free. etc)!
    RawUniqueTransportData<T> raw_unique_ptr{ std::move(*data.m_data) /*.first*/, [](void* ptr) { free(ptr); } };
    return raw_unique_ptr;
}