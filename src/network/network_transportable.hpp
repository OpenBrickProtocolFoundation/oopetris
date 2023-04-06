

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
    std::shared_ptr<uint8_t> m_data;
    uint32_t m_data_size;

public:
    RawTransportData(std::uint32_t serialUUID, std::shared_ptr<uint8_t> data, uint32_t data_size);

    // if you change the protocol behaviour in some way, change this number and the number in Transportable (this parses only packets with the same protocol_version)
    static constexpr std::uint32_t protocol_version = 1;

    static tl::expected<std::vector<RawTransportData>, std::string> from_raw_bytes(RawBytes raw_bytes);
    static tl::expected<std::tuple<std::uint32_t, std::uint32_t, std::uint32_t>, std::string> read_header(RawBytes bytes
    );
    static tl::expected<std::uint32_t, std::string> read_checksum(RawBytes bytes, std::uint32_t data_size);

    template<class T>
    bool is_of_type() const {
        return m_serialUUID == T::serialUUID;
    }

    template<class T>
    std::shared_ptr<T> as_type() const {
        if (!is_of_type<T>()) {
            throw std::bad_cast{};
        }

        // using copy constructor, so that this only get's freed, when also the new usage of this raw malloced memory is done
        std::shared_ptr<T> data = std::reinterpret_pointer_cast<T>(m_data);
        return data;
    }
};