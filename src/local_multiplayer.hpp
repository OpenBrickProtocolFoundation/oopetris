

#pragma once

#include "network/network_data.hpp"
#include "network/network_manager.hpp"
#include "play_manager.hpp"
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <string>
#include <tl/expected.hpp>
#include <tl/optional.hpp>
#include <utility>
#include <vector>

using ConnectionStore = std::vector<std::pair<
        std::pair<std::size_t, std::shared_ptr<Connection>>,
        std::vector<std::pair<std::size_t, std::shared_ptr<Connection>>>>>;

struct LocalMultiplayer : public PlayManager {
private:
    std::size_t m_num_players;
    bool m_is_server;
    NetworkManager m_network_manager;
    std::shared_ptr<Server> m_server;
    ConnectionStore m_input_connections;

    tl::expected<std::shared_ptr<Connection>, std::string>
    get_connection_to_server(std::uint32_t delay_between_attempts = 200, std::uint32_t connection_attempts = 10);

public:
    explicit LocalMultiplayer(std::size_t num_players, bool is_server);
    tl::expected<StartState, std::string> init() override;
    std::pair<std::size_t, std::unique_ptr<Input>>
    get_input(std::size_t index, GameManager* associated_game_manager, EventDispatcher* event_dispatcher) override;
};


// little fast helper,
//TODO refactor better and move in some reasonable cpp, and don't use exceptions to handle this
template<class T>
std::shared_ptr<T> await_exact_one(std::shared_ptr<Connection> connection) {
    //TODO make this number customizable
    auto send_initializer = connection->wait_for_data(10 * 1000, 100);
    if (!send_initializer.has_value()) {
        throw std::runtime_error{ "didn't receive data in time: " + send_initializer.error() };
    }

    const auto send_vector = send_initializer.value();
    if (send_vector.size() != 1) {
        throw std::runtime_error{
            "did receive data in time, but also to much data afterwards, it can't be handled "
            "here!"
        };
    }

    const auto received_value = send_vector.at(0);

    if (!received_value.is_of_type<T>()) {
        throw std::runtime_error{ "didn't receive correct data but another serializable message!" };
    }

    return received_value.as_type<T>();
}