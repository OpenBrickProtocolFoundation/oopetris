
#include "network_manager.hpp"
#include "network_transportable.hpp"
#include "network_util.hpp"
#include <SDL_net.h>
#include <memory>
#include <string>
#include <tl/expected.hpp>
#include <tl/optional.hpp>


NetworkManager::NetworkManager() : m_connections{ std::vector<Connection>{} } {};


MaybeConnection NetworkManager::try_connect(const char* host, Uint16 port) {

    IPaddress ip;
    TCPsocket tcpsock;

    if (SDLNet_ResolveHost(&ip, host, port) == -1) {
        std::string error = "SDLNet_ResolveHost: " + network_util::latest_sdl_net_error();
        return tl::make_unexpected(error);
    }

    tcpsock = SDLNet_TCP_Open(&ip);
    if (!tcpsock) {
        std::string error = "SDLNet_TCP_Open: " + network_util::latest_sdl_net_error();
        return tl::make_unexpected(error);
    }

    return std::make_shared<Connection>(tcpsock);
}

MaybeServer NetworkManager::spawn_server(Uint16 port) {

    TCPsocket server;
    IPaddress ip;
    if (SDLNet_ResolveHost(&ip, NULL, port) == -1) {
        std::string error = "SDLNet_ResolveHost: " + network_util::latest_sdl_net_error();
        return tl::make_unexpected(error);
    }
    server = SDLNet_TCP_Open(&ip);
    if (!server) {
        std::string error = "SDLNet_TCP_Open: " + network_util::latest_sdl_net_error();
        return tl::make_unexpected(error);
    }

    return std::make_shared<Server>(server);
}
