
#include "connection_manager.hpp"
#include "network_manager.hpp"
#include "network_transportable.hpp"
#include <SDL.h>
#include <tl/optional.hpp>


Connection::Connection(TCPsocket socket)
    : m_socket{ socket } {

      };

Connection::~Connection() {
    SDLNet_TCP_Close(m_socket);
}

tl::optional<std::string> Connection::send_data(const Transportable transportable) {


    auto [message, length] = Transportable::serialize(transportable);

    const auto result = SDLNet_TCP_Send(m_socket, message, length);
    if (result == -1) {
        return tl::make_optional("SDLNet_TCP_Send: invalid socket");
    }

    if ((std::size_t) result != length) {
        std::string error = "SDLNet_TCP_Send: " + std::string{ SDLNet_GetError() };
        return tl::make_optional(error);
    }

    return {};
};


Server::Server(TCPsocket socket) : m_socket{ socket } {};

Server::~Server() {
    SDLNet_TCP_Close(m_socket);
}


tl::optional<Connection> Server::try_get_client() {

    TCPsocket client;
    /* try to accept a connection */
    client = SDLNet_TCP_Accept(m_socket);
    if (client) { /* no connection accepted */
        return Connection{ client };
    }
    return {};
}

tl::optional<Connection> Server::get_client(std::size_t ms_delay, std::size_t abort_after) {
    auto start_time = SDL_GetTicks64();
    while (true) {
        /* try to accept a connection */
        auto client = this->try_get_client();
        if (client.has_value()) {
            return client;
        }

        auto elapsed_time = SDL_GetTicks64() - start_time;
        if (elapsed_time >= abort_after) {
            return {};
        }

        SDL_Delay(ms_delay);
        continue;
    }
}