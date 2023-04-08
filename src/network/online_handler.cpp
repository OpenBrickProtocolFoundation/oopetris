

#include "online_handler.hpp"
#include "../input.hpp"
#include "connection_manager.hpp"
#include "network_data.hpp"
#include <cstdint>
#include <stdexcept>

OnlineHandler::OnlineHandler(std::shared_ptr<Server> server, std::shared_ptr<Connection> connection)
    : m_server{ server },
      m_connection{ connection },
      m_send_to{ std::vector<std::shared_ptr<Connection>>{} } {};


OnlineHandler::OnlineHandler(
        std::shared_ptr<Server> server,
        std::shared_ptr<Connection> connection,
        std::vector<std::shared_ptr<Connection>> send_to
)
    : m_server{ server },
      m_connection{ connection },
      m_send_to{ send_to } {};


void OnlineHandler::handle_event(InputEvent event) {
    if (m_server) {
        auto event_data = EventData{ event };
        //TODO handle error
        m_server->send_all<EventData>(&event_data, m_send_to);

    } else if (m_connection) {
        auto event_data = EventData{ event };
        //TODO handle error
        m_connection->send_data<EventData>(&event_data);
    } else {
        throw std::runtime_error{ "OnlineHandler needs either a connection (client mode) or server!" };
    }
}