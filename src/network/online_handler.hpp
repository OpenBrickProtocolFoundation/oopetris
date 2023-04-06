
#pragma once

#include "../input.hpp"
#include "connection_manager.hpp"
#include <cstdint>

struct OnlineHandler {
private:
    std::shared_ptr<Server> m_server;
    std::shared_ptr<Connection> m_connection;
    std::uint32_t m_uuid;

public:
    OnlineHandler(std::shared_ptr<Server> server, std::shared_ptr<Connection> connection, std::uint32_t uuid);
    void handle_event(Input::Event event);
};