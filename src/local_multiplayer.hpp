

#pragma once

#include "network/network_manager.hpp"
#include "play_manager.hpp"
#include <cstddef>
#include <memory>
#include <string>
#include <tl/expected.hpp>
#include <tl/optional.hpp>

struct LocalMultiplayer : public PlayManager {
private:
    std::size_t m_num_players;
    bool m_is_server;
    NetworkManager m_network_manager;
    std::shared_ptr<Server> m_server;

    tl::expected<std::shared_ptr<Connection>, std::string>
    get_connection_to_server(std::uint32_t delay_between_attempts = 200, std::uint32_t connection_attempts = 10);

public:
    explicit LocalMultiplayer(std::size_t num_players, bool is_server);
    std::size_t get_num_players() override;
    tl::optional<std::string> init() override;
    std::unique_ptr<Input>
    get_input(std::size_t index, GameManager* associated_game_manager, EventDispatcher* event_dispatcher) override;
};
