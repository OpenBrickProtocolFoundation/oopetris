#pragma once

#include "application.hpp"
#include "game_manager.hpp"
#include "tetromino_type.hpp"

struct TetrisApplication : public Application {
public:
    static constexpr int width = 800;
    static constexpr int height = 600;

    TetrisApplication() : Application{ "TetrisApplication", WindowPosition::Centered, width, height } { }

private:
    GameManager m_game;

protected:
    bool handle_event(const SDL_Event& event) override {
        // the parent implementation handles the escape key and the "close window" button
        if (Application::handle_event(event)) {
            return true;
        }
        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_s:
                    m_game.spawn_tetromino();
                    return true;
                case SDLK_a:
                    m_game.rotate_tetromino_left();
                    return true;
                case SDLK_d:
                    m_game.rotate_tetromino_right();
                    return true;
            }
        }
        return false;
    }

    void update(double delta_time) override { }

    void render() const override {
        Application::render(); // call parent function to clear the screen
        m_game.render(*this);
    }
};
