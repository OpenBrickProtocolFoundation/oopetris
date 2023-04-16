
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
    Input::OnEventCallback m_on_event_callback;

    void handle_event(InputEvent event, u64 simulation_step_index);

public:
    OnlineHandler(
            std::shared_ptr<Server> server,
            std::shared_ptr<Connection> connection,
            Input::OnEventCallback on_event_callback
    );
    OnlineHandler(
            std::shared_ptr<Server> server,
            std::shared_ptr<Connection> connection,
            std::vector<std::shared_ptr<Connection>> send_to,
            Input::OnEventCallback on_event_callback
    );
    Input::OnEventCallback get_callback_function();
};