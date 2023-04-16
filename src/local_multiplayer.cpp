
#include "local_multiplayer.hpp"
#include "network/network_data.hpp"
#include "network/network_manager.hpp"
#include "network/online_input.hpp"
#include "play_mode.hpp"
#include "tetrion.hpp"
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <string>
#include <tl/expected.hpp>
#include <tl/optional.hpp>
#include <utility>
#include <variant>
#include <vector>

LocalMultiplayer::LocalMultiplayer(const bool is_replay_mode, const std::size_t num_players, const bool is_server)
    : PlayMode{ is_replay_mode },
      m_num_players{ num_players },
      m_is_server{ is_server },
      m_network_manager{ NetworkManager{} },
      m_server{ nullptr },
      m_input_connections{ ConnectionStore{} } {};


tl::expected<StartState, std::string> LocalMultiplayer::init(Settings settings, Random::Seed seed) {
    PlayMode::init(settings, seed);

    if (m_is_server) {

        if (m_num_players > 4 || m_num_players < 2) {
            return tl::make_unexpected(
                    "The LocalMultiplayer mode only allows between 2 and 4 players, but got: "
                    + std::to_string(m_num_players)
            );
        }

        if (settings.controls.size() < m_num_players) {
            return tl::make_unexpected(
                    "Not enough controls provided: needed: " + std::to_string(m_num_players)
                    + " got: " + std::to_string(settings.controls.size())
            );
        }

        auto server = m_network_manager.spawn_server();
        if (!server.has_value()) {
            return tl::make_unexpected("Error in initializing the server: " + server.error());
        }

        m_server = server.value();
        m_input_connections.reserve(m_num_players - 1);

        //TODO in here refactor some routines to separate functions!
        for (std::size_t i = 0; i < m_num_players - 1; ++i) {
            auto client = m_server->get_client();
            if (!client.has_value()) {
                throw std::runtime_error{ "Waited to long for new client nr. " + std::to_string(i) };
            }

            const auto connection = client.value();


            auto send_initializer = connection->wait_for_data(10 * 1000, 100);
            if (!send_initializer.has_value()) {
                throw std::runtime_error{ "client nr. " + std::to_string(i)
                                          + " didn't send InitializationData in time: " + send_initializer.error() };
            }

            const auto send_vector = send_initializer.value();
            if (send_vector.size() != 1) {
                throw std::runtime_error{
                    "client nr. "+ std::to_string(i)+" did send InitializationData in time, but also to much data afterwards, it can't be handled "
                    "here!"
                };
            }

            const auto received_value = send_vector.at(0);

            if (!received_value.is_of_type<InitializationData>()) {
                throw std::runtime_error{ "client nr. " + std::to_string(i)
                                          + " didn't send InitializationData but another serializable message!" };
            }

            const auto initializer_message = received_value.as_type<InitializationData>();

            if (initializer_message->type == InitializationDataType::Send) {

                if (initializer_message->uuid != 0) {
                    throw std::runtime_error{ "Wrong InitializationData: first connection must have uuid 0 but has "
                                              + std::to_string(initializer_message->uuid) };
                }

                //this is correct fro online_input, set this client connection as player index, query conenction for info like starting piece and eventual other information

                //check if client game version is compatible with this game, otherwise reject (with a message to teh client and to the server host!)

                // send the information about how many players partecipate in response, await the creation of x-1 sockets to send them the event from them, not that the client doesn't have a static player number, only the server can (later select this from a menu) set this size


                auto send_data = ClientInitializationData{ static_cast<std::uint32_t>(m_num_players),
                                                           static_cast<std::uint32_t>(i + 1), seed };
                const auto send_result = connection->send_data<ClientInitializationData>(&send_data);
                if (send_result.has_value()) {
                    throw std::runtime_error{ "ClientInitializationData failed to send" + send_result.value() };
                }


                auto receive_connections = std::vector<std::pair<std::size_t, std::shared_ptr<Connection>>>{};
                receive_connections.reserve(m_num_players - 1);

                for (std::size_t j = 0; j < m_num_players - 1; ++j) {
                    auto receive_client = m_server->get_client();
                    if (!receive_client.has_value()) {
                        throw std::runtime_error{ "Waited to long for new receive client nr. " + std::to_string(j) };
                    }

                    const auto receive_connection = receive_client.value();


                    auto receive_send_initializer = receive_connection->wait_for_data(10 * 1000, 100);
                    if (!receive_send_initializer.has_value()) {
                        throw std::runtime_error{ "receive client nr. " + std::to_string(j)
                                                  + " didn't send InitializationData in time: "
                                                  + receive_send_initializer.error() };
                    }

                    const auto receive_send_vector = receive_send_initializer.value();
                    if (send_vector.size() != 1) {
                        throw std::runtime_error{
                    "receive client nr. "+ std::to_string(j)+" did send InitializationData in time, but also to much data afterwards, it can't be handled "
                    "here!"
                };
                    }

                    const auto receive_received_value = receive_send_vector.at(0);

                    if (!receive_received_value.is_of_type<InitializationData>()) {
                        throw std::runtime_error{
                            "receive client nr. " + std::to_string(j)
                            + " didn't send InitializationData but another serializable message!"
                        };
                    }

                    const auto receive_initializer_message = receive_received_value.as_type<InitializationData>();

                    if (receive_initializer_message->type == InitializationDataType::Receive) {

                        receive_connections.emplace_back(receive_initializer_message->uuid, receive_connection);

                    } else {
                        throw std::runtime_error{
                            "receive client didn't send InitializationDataType::Receive as first message!"
                        };
                    }
                }


                m_input_connections.emplace_back(
                        std::pair<std::size_t, std::shared_ptr<Connection>>{ 0, connection }, receive_connections
                );
            } else {
                throw std::runtime_error{ "client didn't send InitializationDataType::Send as first message!" };
            }
        }


        return StartState{ m_num_players, seed };
    } else {
        // client start here

        auto connection_result = get_connection_to_server();

        if (!connection_result.has_value()) {
            throw std::runtime_error{ connection_result.error() };
        }

        auto connection = connection_result.value();
        auto send_data = InitializationData{ InitializationDataType::Send, static_cast<std::uint32_t>(0) };
        const auto send_result = connection->send_data<InitializationData>(&send_data);
        if (send_result.has_value()) {
            throw std::runtime_error{ "InitializationData failed to send" + send_result.value() };
        }

        const auto data = await_exact_one<ClientInitializationData>(connection);
        const auto num_players = data->player_num;
        m_num_players = num_players;

        auto receive_connections = std::vector<std::pair<std::size_t, std::shared_ptr<Connection>>>{};
        receive_connections.reserve(num_players - 1);


        const auto my_player_id = data->your_player_id;

        for (std::uint32_t i = 0; i < num_players; ++i) {
            if (i == my_player_id) {
                continue;
            }

            auto receive_connection_result = get_connection_to_server();

            if (!receive_connection_result.has_value()) {
                throw std::runtime_error{ receive_connection_result.error() };
            }

            auto receive_connection = receive_connection_result.value();
            auto receive_send_data =
                    InitializationData{ InitializationDataType::Receive, static_cast<std::uint32_t>(i) };
            const auto receive_send_result = receive_connection->send_data<InitializationData>(&receive_send_data);
            if (receive_send_result.has_value()) {
                throw std::runtime_error{ "InitializationData failed to send" + receive_send_result.value() };
            }

            receive_connections.emplace_back(i, receive_connection);
        }

        m_input_connections.emplace_back(
                std::pair<std::size_t, std::shared_ptr<Connection>>{ static_cast<std ::size_t>(my_player_id),
                                                                     connection },
                receive_connections
        );


        return StartState{ num_players, data->seed };
    }
}


std::unique_ptr<Input> LocalMultiplayer::get_input(
        u8 index,
        Tetrion* tetrion,
        Input::OnEventCallback event_callback,
        EventDispatcher* event_dispatcher
) {
    if (index >= m_num_players) {
        throw std::range_error{ "LocalMultiplayer mode: error in index of get_input" };
    }

    if (index == 0) {
        if (m_is_server) {

            constexpr std::size_t my_id = 0;
            auto send_to = std::vector<std::shared_ptr<Connection>>{};
            for (const auto& client_bundle : m_input_connections) {

                for (const auto& [num, receive_connection] : client_bundle.second) {
                    if (my_id == num) {
                        send_to.push_back(receive_connection);
                    }
                }
            }

            m_online_handlers.push_back(
                    std::make_unique<OnlineHandler>(m_server, nullptr, send_to, std::move(event_callback))
            );

            auto modified_event_callback = m_online_handlers.back()->get_callback_function();

            tetrion->set_player_num(0);
            return utils::create_input(
                    settings().controls.at(index), tetrion, std::move(modified_event_callback), event_dispatcher
            );

            /*    tetrion->set_player_num(0);
            return TetrisApplication::create_input(
                    settings().controls.at(index), tetrion, std::move(event_callback), event_dispatcher
            );
 */
        } else {

            auto connection_result = get_connection_to_server();

            if (m_input_connections.size() != 1) {
                throw std::runtime_error{
                    "LocalMultiplayer::init was implemented wrong, client only has one value in m_input_connections, "
                    "but were: "
                    + std::to_string(m_input_connections.size())
                };
            }

            m_online_handlers.push_back(std::make_unique<OnlineHandler>(
                    nullptr, m_input_connections.at(0).first.second, std::move(event_callback)
            ));


            auto modified_event_callback = m_online_handlers.back()->get_callback_function();

            tetrion->set_player_num(m_input_connections.at(0).first.first);
            return utils::create_input(
                    settings().controls.at(index), tetrion, std::move(modified_event_callback), event_dispatcher
            );
        }
    } else {

        if (m_is_server) {

            auto online_input = std::make_unique<OnlineInput>(tetrion, m_input_connections.at(index - 1).first.second);
            tetrion->set_player_num(index);
            return online_input;
        } else {

            if (m_input_connections.size() != 1) {
                throw std::runtime_error{
                    "LocalMultiplayer::init was implemented wrong, client only has one value in m_input_connections, "
                    "but were: "
                    + std::to_string(m_input_connections.size())
                };
            }

            std::shared_ptr<Connection> connection = nullptr;
            std::size_t player_num = 0;
            const auto my_id = m_input_connections.at(0).first.first;
            for (auto [num, receive_connection] : m_input_connections.at(0).second) {
                const u8 actual_index = index <= my_id ? index - 1 : index;
                if (actual_index == num) {
                    connection = receive_connection;
                    player_num = num;
                    break;
                }
            }

            if (!connection) {
                throw std::runtime_error{ "fatal initialization error, no connection for index " + std::to_string(index)
                                          + " found!" };
            }


            auto online_input = std::make_unique<OnlineInput>(tetrion, connection);

            tetrion->set_player_num(player_num);
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
