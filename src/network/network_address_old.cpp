
#include "network_address.hpp"
#include "network_transportable.hpp"
#include <SDL_net.h>
#include <string>
#include <tl/expected.hpp>
#include <tl/optional.hpp>

NetworkAddress::NetworkAddress(TCPsocket socket) : m_socket{ std::move(socket) } { }

NetworkAddress::~NetworkAddress() {
    SDLNet_TCP_Close(m_socket);
}

tl::optional<std::string> NetworkAddress::send_data(RawBytes bytes) {
    auto [message, length] = bytes;
    const auto result = SDLNet_TCP_Send(m_socket, message, length);
    if (result == -1) {
        return tl::make_optional("SDLNet_TCP_Send: invalid socket");
    }

    if (result != length) {
        std::string error = "SDLNet_TCP_Send: " + std::string{ SDLNet_GetError() };
        return tl::make_optional(error);
    }

    return {};
}


tl::optional<Connection> NetworkAddress::try_accept() {

    TCPsocket client;
    /* try to accept a connection */
    client = SDLNet_TCP_Accept(m_socket);
    if (client) { /* no connection accepted */
        return Connection{ client };
    }
    return {};
}
