#pragma once

#include "application.hpp"
#include "game_manager.hpp"
#include "tetromino_type.hpp"

struct TetrisApplication : public Application {
private:
    GameManager m_game;

public:
    static constexpr int width = 800;
    static constexpr int height = 600;

    TetrisApplication() : Application{ "TetrisApplication", WindowPosition::Centered, width, height } {
        m_game.spawn_next_tetromino();
    }

protected:
    bool handle_event(const SDL_Event& event) override {
        // the parent implementation handles the escape key and the "close window" button
        if (Application::handle_event(event)) {
            return true;
        }
        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_LEFT:
                    m_game.rotate_tetromino_left();
                    return true;
                case SDLK_RIGHT:
                    m_game.rotate_tetromino_right();
                    return true;
                case SDLK_s:
                    m_game.move_tetromino_down(MovementType::Forced);
                    return true;
                case SDLK_a:
                    m_game.move_tetromino_left();
                    return true;
                case SDLK_d:
                    m_game.move_tetromino_right();
                    return true;
                case SDLK_SPACE:
                case SDLK_w:
                    m_game.drop_tetromino();
                    return true;
            }
        }
        return false;
    }

    void update(double) override {
        m_game.update();
    }

    void render() const override {
        Application::render(); // call parent function to clear the screen
        m_game.render(*this);
    }
};
