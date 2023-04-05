


#pragma once


#include "network_transportable.hpp"
#include <SDL_net.h>
#include <string>
#include <tl/optional.hpp>


struct Connection {

private:
    TCPsocket m_socket;

public:
    explicit Connection(TCPsocket socket);
    ~Connection();
    tl::optional<std::string> send_data(const Transportable transportable);
};


struct Server {

private:
    TCPsocket m_socket;

public:
    explicit Server(TCPsocket socket);
    ~Server();
    tl::optional<Connection> try_get_client();
    tl::optional<Connection> get_client(std::size_t ms_delay = 100, std::size_t abort_after = 60 * 1000);
};
