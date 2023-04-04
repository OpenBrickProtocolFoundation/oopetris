#pragma once

#include "application.hpp"
#include "game_manager.hpp"
#include "tetromino_type.hpp"
#include <cassert>
#include <iostream>

struct TetrisApplication : public Application {
private:
    enum class InputMethod {
        Keyboard,
    };

    std::vector<std::unique_ptr<GameManager>> m_game_managers;
    std::vector<std::unique_ptr<Input>> m_inputs;

public:
    static constexpr int width = 800;
    static constexpr int height = 600;

    TetrisApplication() : Application{ "TetrisApplication", WindowPosition::Centered, width, height } {
        static constexpr auto num_players = 1;
        for (int i = 0; i < num_players; ++i) {
            m_game_managers.push_back(std::make_unique<GameManager>());
            m_inputs.push_back(create_input(InputMethod::Keyboard, m_game_managers.back().get()));
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
        }
        assert(false and "unreachable");
        return {};
    }
};
