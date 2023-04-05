#pragma once

#include "application.hpp"
#include "game_manager.hpp"
#include "network/network_manager.hpp"
#include "tetromino_type.hpp"
#include <cassert>
#include <exception>
#include <iostream>
#include <utility>


enum class RunType { OnlineMultiplayer, LocalMultiplayer, SinglePlayer, VersusKI };

struct TetrisApplication : public Application {
private:
    enum class InputMethod { Keyboard, OnlineNetwork, LocalNetwork, KI };

    std::vector<std::unique_ptr<GameManager>> m_game_managers;
    std::vector<std::unique_ptr<Input>> m_inputs;
    NetworkManager m_network_manager;
    RunType m_run_type;
    int m_num_players;

public:
    static constexpr int width = 800;
    static constexpr int height = 600;

    TetrisApplication(RunType run_type, int num_players)
        : Application{ "TetrisApplication", WindowPosition::Centered, width, height },
          m_network_manager{ NetworkManager{} },
          m_run_type{ run_type },
          m_num_players{ num_players } {

        auto input_methods = std::vector<InputMethod>{};
        switch (run_type) {
            case RunType::OnlineMultiplayer: {

                if (num_players < 2 || num_players > 4) {
                    throw std::range_error{ "The mode OnlineMultiplayer only allows between 2 and 4 players" };
                }
                input_methods.push_back(InputMethod::Keyboard);
                for (int i = 0; i < num_players - 1; ++i) {
                    input_methods.push_back(InputMethod::OnlineNetwork);
                }
                break;
            }
            case RunType::LocalMultiplayer: {
                if (num_players < 2 || num_players > 4) {
                    throw std::range_error{ "The mode OnlineMultiplayer only allows between 2 and 4 players" };
                }
                input_methods.push_back(InputMethod::Keyboard);
                for (int i = 0; i < num_players - 1; ++i) {
                    input_methods.push_back(InputMethod::LocalNetwork);
                }
                break;
            }
            case RunType::SinglePlayer: {
                if (num_players != 1) {
                    throw std::range_error{ "The mode SinglePlayer only allows 1 player" };
                }
                input_methods.push_back(InputMethod::Keyboard);
                break;
            }
            case RunType::VersusKI: {
                if (num_players < 2 || num_players > 4) {
                    throw std::range_error{ "The mode VersusKI only allows between 2 and 4 players" };
                }
                input_methods.push_back(InputMethod::Keyboard);
                for (int i = 0; i < num_players - 1; ++i) {
                    input_methods.push_back(InputMethod::KI);
                }
                break;
            }
        }

        for (int i = 0; i < num_players; ++i) {
            m_game_managers.push_back(std::make_unique<GameManager>());
            // TODO, create input has to connect the players for online mode, that process has to be made better and clearer (with waiting screen / lobby)
            m_inputs.push_back(create_input(input_methods.at(i), m_game_managers.back().get()));
        }
        for (const auto& game_manager : m_game_managers) {
            game_manager->spawn_next_tetromino();
        }
    }

protected:
    void update(double) override {
        for (const auto& input : m_inputs) {
            input->update();
        }

        for (const auto& game_manager : m_game_managers) {
            game_manager->update();
        }
    }

    void render() const override {
        Application::render(); // call parent function to clear the screen
        for (const auto& game_manager : m_game_managers) {
            game_manager->render(*this);
        }
    }

private:
    std::unique_ptr<Input> create_input(InputMethod input_method, GameManager* associated_game_manager) {
        switch (input_method) {
            case InputMethod::Keyboard: {
                auto keyboard_input = std::make_unique<KeyboardInput>(associated_game_manager);
                m_event_dispatcher.register_listener(keyboard_input.get());
                return keyboard_input;
            }
            case InputMethod::OnlineNetwork: {
                MaybeConnection connection = m_network_manager.try_connect();
                if (connection.has_value()) {
                    auto online_input = std::make_unique<OnlineInput>(associated_game_manager, connection.value());
                    return online_input;
                }
                std::cout << "Error in getting a connection for InputMethod::OnlineNetwork: " << connection.error()
                          << "\n";
                return {};
            }
            case InputMethod::LocalNetwork: {
                assert(false and "unreachable");
                return {};
            }
            case InputMethod::KI: {
                assert(false and "unreachable");
                return {};
            }
        }
        assert(false and "unreachable");
        return {};
    }
};
