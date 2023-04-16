
#include "online_input.hpp"
#include "../application.hpp"


void OnlineInput::get_data() {
    auto data = m_connection->get_data();
    if (!data.has_value()) {
        // TODO: print error here (to log e.g.)
        // auto error = data.error();
        return;
    }

    if (!data.value().has_value()) {
        // no data given
        return;
    }

    const auto data_vector = data.value().value();
    for (const auto& received_data : data_vector) {

        if (received_data.is_of_type<EventData>()) {
            auto data = received_data.as_type<EventData>();
            //TODO maybe handle return value ?
            m_data.push_back(data);
        }
    }
}

void OnlineInput::update() {
    //TODO the connection should now if it's disconnected
    /* if (not m_connection.is_disconnected()) {
            break;
        } */

    get_data();

    const auto current_application_simulation_step = Application::simulation_step_index();
    for (const auto& data : m_data) {

        //TODO: synchronize so that if I receive an event that is 2 frames behind it has to be displayed nontheless, atm only start snychronization is needed
        //  const auto is_record_for_current_step = (data->simulation_step_index == current_application_simulation_step);

        if (data->simulation_step_index > current_application_simulation_step) {
            continue;
        }

        Input::handle_event(data->event);
    }
    // remove all old data
    //TODO improve, that it doesn't discard to much, e.g. wait if it doesn't receive data as quickly
    m_data.erase(
            std::remove_if(
                    m_data.begin(), m_data.end(),
                    [&](const auto& data) { return data->simulation_step_index <= current_application_simulation_step; }
            ),
            m_data.end()
    );


    Input::update();
}
