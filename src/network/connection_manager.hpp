


#pragma once


#include "network_transportable.hpp"
#include <SDL_net.h>
#include <memory>
#include <string>
#include <tl/expected.hpp>
#include <tl/optional.hpp>
#include <vector>


using MaybeData = tl::expected<tl::optional<std::vector<RawTransportData>>, std::string>;


struct Connection {

private:
    TCPsocket m_socket;

    static constexpr std::size_t chunk_size = 1024;

public:
    explicit Connection(TCPsocket socket);
    ~Connection();
    tl::optional<std::string> send_data(const Transportable* transportable, std::uint32_t data_size);

    tl::expected<bool, std::string> is_data_available(Uint32 timeout_ms = 3);
    tl::expected<RawBytes, std::string> get_all_data_blocking();
    MaybeData get_data();

    template<class T>
    tl::optional<std::string> send_data(const T* transportable) {
        return send_data(transportable, sizeof(T));
    }
};


struct Server {

private:
    TCPsocket m_socket;
    std::vector<std::shared_ptr<Connection>> m_connections;

public:
    explicit Server(TCPsocket socket);
    ~Server();
    tl::optional<std::shared_ptr<Connection>> try_get_client();
    tl::optional<std::shared_ptr<Connection>> get_client(Uint32 ms_delay = 100, std::size_t abort_after = 60 * 1000);
    tl::optional<std::string> send_all(const Transportable* transportable, std::uint32_t data_size);

    template<class T>
    tl::optional<std::string> send_all(const T* transportable) {
        return send_all(transportable, sizeof(T));
    }
};
