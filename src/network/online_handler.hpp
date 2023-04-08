
#pragma once

#include "../input.hpp"
#include "connection_manager.hpp"
#include <cstdint>
#include <vector>

struct OnlineHandler {
private:
    std::shared_ptr<Server> m_server;
    std::shared_ptr<Connection> m_connection;
    std::vector<std::shared_ptr<Connection>> m_send_to;

public:
    OnlineHandler(std::shared_ptr<Server> server, std::shared_ptr<Connection> connection);
    OnlineHandler(
            std::shared_ptr<Server> server,
            std::shared_ptr<Connection> connection,
            std::vector<std::shared_ptr<Connection>> send_to
    );
    void handle_event(InputEvent event);
};