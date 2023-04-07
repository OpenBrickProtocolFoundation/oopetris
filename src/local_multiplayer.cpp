
#include "local_multiplayer.hpp"
#include "network/network_data.hpp"
#include "network/network_manager.hpp"
#include "play_manager.hpp"
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <string>
#include <tl/expected.hpp>
#include <tl/optional.hpp>

LocalMultiplayer::LocalMultiplayer(std::size_t num_players, bool is_server)
    : PlayManager{},
      m_num_players{ num_players },
      m_is_server{ is_server },
      m_network_manager{ NetworkManager{} },
      m_server{ nullptr } {};

std::size_t LocalMultiplayer::get_num_players() {
    return m_num_players;
};

tl::optional<std::string> LocalMultiplayer::init() {
    if (m_num_players > 4 || m_num_players < 2) {
        return tl::make_optional(
                "The LocalMultiplayer mode only allows between 2 and 4 players, but got: "
                + std::to_string(m_num_players)
        );
    }

    if (m_is_server) {
        auto server = m_network_manager.spawn_server();
        if (!server.has_value()) {
            return tl::make_optional("Error in initializing the server: " + server.error());
        }

        m_server = server.value();
    }

    return {};
}


std::unique_ptr<Input> LocalMultiplayer::get_input(
        std::size_t index,
        GameManager* associated_game_manager,
        EventDispatcher* event_dispatcher
) {
    if (index >= m_num_players) {
        throw std::range_error{ "LocalMultiplayer mode: error in index of get_input" };
    }

    if (index == 0) {
        if (m_is_server) {

            associated_game_manager->set_online_handler(std::make_unique<OnlineHandler>(m_server, nullptr, 0));

            auto keyboard_input = std::make_unique<KeyboardInput>(associated_game_manager);
            event_dispatcher->register_listener(keyboard_input.get());
            return keyboard_input;
        } else {


            auto connection_result = get_connection_to_server();

            if (!connection_result.has_value()) {
                throw std::runtime_error{ connection_result.error() };
            }

            auto connection = connection_result.value();

            associated_game_manager->set_online_handler(std::make_unique<OnlineHandler>(nullptr, connection, 0));

            auto keyboard_input = std::make_unique<KeyboardInput>(associated_game_manager);
            event_dispatcher->register_listener(keyboard_input.get());
            return keyboard_input;
        }
    } else {

        if (m_is_server) {
            auto client = m_server->get_client();
            if (!client.has_value()) {
                throw std::runtime_error{ "Waited to long for new client" };
            }

            const auto connection = client.value();
            auto online_input = std::make_unique<OnlineInput>(associated_game_manager, connection);

            auto send_initializer = connection->wait_for_data(10 * 1000, 100);
            if (!send_initializer.has_value()) {
                throw std::runtime_error{ "client didn't send InitializationData in time: "
                                          + send_initializer.error() };
            }


            return online_input;
        } else {

            auto connection_result = get_connection_to_server();

            if (!connection_result.has_value()) {
                throw std::runtime_error{ connection_result.error() };
            }

            auto connection = connection_result.value();
            auto online_input = std::make_unique<OnlineInput>(associated_game_manager, connection);
            auto send_data = InitializationData{ InitializationDataType::Client, (std::uint32_t) index };
            const auto send_result = connection->send_data<InitializationData>(&send_data);
            if (send_result.has_value()) {
                throw std::runtime_error{ "InitializationData failed to send" + send_result.value() };
            }
            return online_input;
        }
    }
}


tl::expected<std::shared_ptr<Connection>, std::string>
LocalMultiplayer::get_connection_to_server(std::uint32_t delay_between_attempts, std::uint32_t connection_attempts) {


    for (std::uint32_t i = 0; i < connection_attempts; ++i) {
        MaybeConnection connection = m_network_manager.try_connect();
        if (connection.has_value()) {
            return connection.value();
        }
        SDL_Delay(delay_between_attempts);
    }

    return tl::make_unexpected(
            "Error in getting a connection for LocalMultiplayer: failed after " + std::to_string(connection_attempts)
            + " attempts"
    );
}