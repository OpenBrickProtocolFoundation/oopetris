
#include "network_manager.hpp"
#include "network_transportable.hpp"
#include <SDL_net.h>
#include <string>
#include <tl/expected.hpp>
#include <tl/optional.hpp>


NetworkManager::NetworkManager() : m_connections{ std::vector<Connection>{} } {};


MaybeConnection NetworkManager::try_connect(const char* host, std::size_t port) {

    IPaddress ip;
    TCPsocket tcpsock;

    if (SDLNet_ResolveHost(&ip, host, port) == -1) {
        std::string error = "SDLNet_ResolveHost: " + std::string{ SDLNet_GetError() };
        return tl::make_unexpected(error);
    }

    tcpsock = SDLNet_TCP_Open(&ip);
    if (!tcpsock) {
        std::string error = "SDLNet_TCP_Open: " + std::string{ SDLNet_GetError() };
        return tl::make_unexpected(error);
    }

    return Connection{ tcpsock };
}

MaybeServer NetworkManager::spawn_server(std::size_t port) {

    TCPsocket server;
    IPaddress ip;
    if (SDLNet_ResolveHost(&ip, NULL, port) == -1) {
        std::string error = "SDLNet_ResolveHost: " + std::string{ SDLNet_GetError() };
        return tl::make_unexpected(error);
    }
    server = SDLNet_TCP_Open(&ip);
    if (!server) {
        std::string error = "SDLNet_TCP_Open: " + std::string{ SDLNet_GetError() };
        return tl::make_unexpected(error);
    }

    return Server{ server };
}
