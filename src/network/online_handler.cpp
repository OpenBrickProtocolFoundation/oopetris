

#include "online_handler.hpp"
#include "../input.hpp"
#include "connection_manager.hpp"
#include "network_data.hpp"
#include <cstdint>
#include <stdexcept>

OnlineHandler::OnlineHandler(std::shared_ptr<Server> server, std::shared_ptr<Connection> connection, std::uint32_t uuid)
    : m_server{ server },
      m_connection{ connection },
      m_uuid{ uuid } {};


void OnlineHandler::handle_event(Input::Event event) {
    if (m_server) {
        auto event_data = EventData{ event };
        //TODO handle error
        ptr_server_send_all(m_server, &event_data);

        /*  m_server.send_all_if(EventData{ event }, []() {


        }); */
    } else if (m_connection) {
        auto event_data = EventData{ event };
        //TODO handle error
        ptr_connection_send_data(m_connection, &event_data);
    } else {
        throw std::runtime_error{ "OnlineHandler needs either a connection (client mode) or server!" };
    }
}