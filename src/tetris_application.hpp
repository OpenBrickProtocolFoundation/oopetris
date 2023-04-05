#pragma once

#include "application.hpp"
#include "game_manager.hpp"
#include "network/network_manager.hpp"
#include "play_manager.hpp"
#include "tetromino_type.hpp"
#include <cassert>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <memory>
#include <utility>


struct TetrisApplication : public Application {
private:
    enum class InputMethod { Keyboard, OnlineNetwork, LocalNetwork, KI };

    std::vector<std::unique_ptr<GameManager>> m_game_managers;
    std::vector<std::unique_ptr<Input>> m_inputs;

    std::unique_ptr<PlayManager> m_manager;

public:
    static constexpr int width = 800;
    static constexpr int height = 600;

    TetrisApplication(std::unique_ptr<PlayManager> manager)
        : Application{ "TetrisApplication", WindowPosition::Centered, width, height },
          m_manager{ std::move(manager) } {

        auto is_valid = m_manager->init();
        if (is_valid.has_value()) {
            std::cerr << "Error in initializing PlayManager: " << is_valid.value() << "\n";
            std::exit(2);
        }

        auto num_players = m_manager->get_num_players();

        for (std::size_t i = 0; i < num_players; ++i) {
            m_game_managers.push_back(std::make_unique<GameManager>());
            std::cout << "initializing manager input at" << i << " (online atm)\n";
            m_inputs.push_back(m_manager->get_input(i, m_game_managers.back().get(), m_event_dispatcher));
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
    //
};
