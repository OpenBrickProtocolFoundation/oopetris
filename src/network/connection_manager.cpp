
#include "connection_manager.hpp"
#include "network_manager.hpp"
#include "network_transportable.hpp"
#include <SDL.h>
#include <cstdlib>
#include <memory>
#include <string>
#include <tl/optional.hpp>
#include <vector>

Connection::Connection(TCPsocket socket)
    : m_socket{ socket } {

      };

Connection::~Connection() {
    SDLNet_TCP_Close(m_socket);
}

tl::optional<std::string> Connection::send_data(const Transportable* transportable, uint32_t data_size) {

    auto [message, length] = Transportable::serialize(transportable, data_size);

    const auto result = SDLNet_TCP_Send(m_socket, message, length);
    if (result == -1) {
        std::free(message);
        return tl::make_optional("SDLNet_TCP_Send: invalid socket");
    }

    if ((std::size_t) result != length) {
        std::free(message);
        std::string error = "SDLNet_TCP_Send: " + std::string{ SDLNet_GetError() };
        return tl::make_optional(error);
    }

    std::free(message);

    return {};
};


Server::Server(TCPsocket socket) : m_socket{ socket }, m_connections{ std::vector<std::shared_ptr<Connection>>{} } {};

Server::~Server() {
    SDLNet_TCP_Close(m_socket);
}


tl::optional<std::shared_ptr<Connection>> Server::try_get_client() {

    TCPsocket client;
    /* try to accept a connection */
    client = SDLNet_TCP_Accept(m_socket);
    if (client) { /* no connection accepted */
        auto connection = std::make_shared<Connection>(client);
        m_connections.push_back(connection);
        return connection;
    }
    return {};
}

tl::optional<std::shared_ptr<Connection>> Server::get_client(std::size_t ms_delay, std::size_t abort_after) {
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

tl::optional<std::string> Server::send_all(const Transportable* transportable, uint32_t data_size) {

    for (size_t i = 0; i < m_connections.size(); ++i) {
        auto result = m_connections.at(i)->send_data(transportable, data_size);
        if (result.has_value()) {
            return tl::make_optional("Error while sending to client: " + std::to_string(i) + " : " + result.value());
        }
    }

    return {};
}