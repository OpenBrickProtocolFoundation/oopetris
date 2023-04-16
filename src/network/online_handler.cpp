

#include "online_handler.hpp"
#include "../application.hpp"
#include "../input.hpp"
#include "connection_manager.hpp"
#include "network_data.hpp"
#include <cstdint>
#include <stdexcept>

OnlineHandler::OnlineHandler(
        std::shared_ptr<Server> server,
        std::shared_ptr<Connection> connection,
        Input::OnEventCallback on_event_callback
)
    : m_server{ server },
      m_connection{ connection },
      m_send_to{ std::vector<std::shared_ptr<Connection>>{} },
      m_on_event_callback{ std::move(on_event_callback) } {};


OnlineHandler::OnlineHandler(
        std::shared_ptr<Server> server,
        std::shared_ptr<Connection> connection,
        std::vector<std::shared_ptr<Connection>> send_to,
        Input::OnEventCallback on_event_callback
)
    : m_server{ server },
      m_connection{ connection },
      m_send_to{ send_to },
      m_on_event_callback{ std::move(on_event_callback) } {};


void OnlineHandler::handle_event(InputEvent event, u64 simulation_step_index) {
    if (m_server) {
        auto event_data = EventData{ event, simulation_step_index };
        //TODO handle error
        m_server->send_all<EventData>(&event_data, m_send_to);

    } else if (m_connection) {
        auto event_data = EventData{ event, simulation_step_index };
        //TODO handle error
        m_connection->send_data<EventData>(&event_data);
    } else {
        throw std::runtime_error{ "OnlineHandler needs either a connection (client mode) or server!" };
    }
}

Input::OnEventCallback OnlineHandler::get_callback_function() {

    return [this](InputEvent event) {
        this->m_on_event_callback(event);
        this->handle_event(event, Application::simulation_step_index());
    };
}