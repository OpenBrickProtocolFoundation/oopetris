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
    static constexpr int width = 1200;
    static constexpr int height = 600;

    TetrisApplication(std::unique_ptr<PlayManager> manager)
        : Application{ "TetrisApplication", WindowPosition::Centered, width, height },
          m_manager{ std::move(manager) } {

        auto is_valid = m_manager->init();
        if (!is_valid.has_value()) {
            std::cerr << "Error in initializing PlayManager: " << is_valid.error() << "\n";
            std::exit(2);
        }

        auto start_state = is_valid.value();
        const auto num_players = start_state.num_players;
        for (std::size_t i = 0; i < num_players; ++i) {
            //TODO get the start state from the client:
            //   m_game_managers.push_back(std::make_unique<GameManager>(i, start_state.state.at(i)));
            m_game_managers.push_back(std::make_unique<GameManager>(i));
            auto input_pair = m_manager->get_input(i, m_game_managers.back().get(), &m_event_dispatcher);
            m_game_managers.back().get()->set_player_num(input_pair.first);
            m_inputs.push_back(std::move(input_pair.second));
        }
        for (const auto& game_manager : m_game_managers) {
            game_manager->spawn_next_tetromino();
        }

        //TODO if the window is to small to handle num_players, we have to resize in some way ,
        //TODO: if it's to big it has to be resized into an appropiate width
        [[maybe_unused]] const std::size_t game_field_size =
                (GameManager::size_per_field * num_players) + ((num_players - 1) * GameManager::space_between);


        //TODO: resize(), but then game_managers have to updated as well, to repaint or not?
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
