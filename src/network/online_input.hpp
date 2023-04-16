#pragma once

#include "../input.hpp"
#include "../input_event.hpp"
#include "../types.hpp"
#include "connection_manager.hpp"
#include "network_data.hpp"
#include <memory>
#include <vector>

struct OnlineInput : public Input {
private:
    std::shared_ptr<Connection> m_connection;
    std::vector<std::shared_ptr<EventData>> m_data;

    void get_data();

public:
    explicit OnlineInput(Tetrion* tetrion, std::shared_ptr<Connection> connection)
        : Input{ tetrion },
          m_connection{ connection } { }

    void update() override;
};
