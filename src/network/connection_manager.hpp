


#pragma once


#include "network_transportable.hpp"
#include <SDL_net.h>
#include <memory>
#include <string>
#include <tl/optional.hpp>
#include <vector>

struct Connection {

private:
    TCPsocket m_socket;

public:
    explicit Connection(TCPsocket socket);
    ~Connection();
    tl::optional<std::string> send_data(const Transportable* transportable, uint32_t data_size);
};

template<class T>
tl::optional<std::string> connection_send_data(Connection& connection, const T* transportable) {
    return connection.send_data(transportable, sizeof(T));
};

template<class T, class C>
tl::optional<std::string> ptr_connection_send_data(C connection, const T* transportable) {
    return connection->send_data(transportable, sizeof(T));
};


struct Server {

private:
    TCPsocket m_socket;
    std::vector<std::shared_ptr<Connection>> m_connections;

public:
    explicit Server(TCPsocket socket);
    ~Server();
    tl::optional<std::shared_ptr<Connection>> try_get_client();
    tl::optional<std::shared_ptr<Connection>>
    get_client(std::size_t ms_delay = 100, std::size_t abort_after = 60 * 1000);
    tl::optional<std::string> send_all(const Transportable* transportable, uint32_t data_size);
};


template<class T>
tl::optional<std::string> server_send_all(Server& server, const T* transportable) {
    return server.send_all(transportable, sizeof(T));
};

template<class T, class S>
tl::optional<std::string> ptr_server_send_all(S server, const T* transportable) {
    return server->send_all(transportable, sizeof(T));
};
