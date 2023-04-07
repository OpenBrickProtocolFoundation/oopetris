


#pragma once


#include "network_transportable.hpp"
#include "network_util.hpp"
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

    tl::expected<bool, std::string> is_data_available(Uint32 timeout_ms = 3);
    tl::expected<RawBytes, std::string> get_all_data_blocking();
    MaybeData get_data();
    tl::expected<std::vector<RawTransportData>, std::string>
    wait_for_data(std::size_t abort_after = 60 * 1000, Uint32 ms_delay = 100);


    template<class T>
    tl::optional<std::string> send_data(const T* transportable) {


        auto [message, length] = Transportable::serialize<T>(transportable);

        const auto result = SDLNet_TCP_Send(m_socket, message, length);
        if (result == -1) {
            std::free(message);
            return tl::make_optional("SDLNet_TCP_Send: invalid socket");
        }

        if ((std::size_t) result != length) {
            std::free(message);
            std::string error = "SDLNet_TCP_Send: " + network_util::latest_sdl_net_error();
            return tl::make_optional(error);
        }

        std::free(message);

        return {};
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

    template<class T>
    tl::optional<std::string> send_all(const T* transportable, std::vector<std::shared_ptr<Connection>> send_to) {

        for (std::size_t i = 0; i < send_to.size(); ++i) {
            auto result = send_to.at(i)->send_data<T>(transportable);
            if (result.has_value()) {
                return tl::make_optional(
                        "Error while sending to client: " + std::to_string(i) + " : " + result.value()
                );
            }
        }

        return {};
    }

    template<class T>
    tl::optional<std::string> send_all(const T* transportable) {

        return send_all<T>(transportable, m_connections);
    }
};
