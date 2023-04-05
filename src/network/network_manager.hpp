


#pragma once

#include "connection_manager.hpp"
#include <string>
#include <tl/expected.hpp>
#include <tl/optional.hpp>
#include <vector>

enum class NetworkType { Server, Client };

using MaybeConnection = tl::expected<Connection, std::string>;

struct NetworkManager {
private:
    std::vector<Connection> m_connections;
    static constexpr const char* ServerHost = "oopetris.totto.tk";
    static constexpr const int Port = 789;

public:
    explicit NetworkManager();
    MaybeConnection add_connection(NetworkType type);
};