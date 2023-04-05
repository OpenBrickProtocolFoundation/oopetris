


#pragma once

#include "connection_manager.hpp"
#include "network_transportable.hpp"
#include <SDL_net.h>
#include <string>
#include <tl/optional.hpp>

struct NetworkAddress {
private:
    TCPsocket m_socket;

public:
    NetworkAddress(TCPsocket socket);
    ~NetworkAddress();
    // client (Connection) socket only method!
    tl::optional<std::string> send_data(RawBytes bytes);

    // server (Server) socket only method
    tl::optional<Connection> try_accept();
};
