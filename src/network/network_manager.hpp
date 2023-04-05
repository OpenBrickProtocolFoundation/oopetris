


#pragma once

#include "connection_manager.hpp"
#include "network_transportable.hpp"
#include <SDL_net.h>
#include <cstddef>
#include <memory>
#include <string>
#include <tl/expected.hpp>
#include <tl/optional.hpp>
#include <vector>


using MaybeConnection = tl::expected<Connection, std::string>;
using MaybeServer = tl::expected<Server, std::string>;

struct NetworkManager {
private:
    std::vector<Connection> m_connections;
    static constexpr const char* ServerHost = "localhost";
    static constexpr const int Port = 1212;

public:
    explicit NetworkManager();
    MaybeConnection try_connect(const char* host = NetworkManager::ServerHost, std::size_t port = NetworkManager::Port);
    MaybeServer spawn_server(std::size_t port = NetworkManager::Port);
};
