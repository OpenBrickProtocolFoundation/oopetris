#pragma once

#include "grid.hpp"
#include "tetromino.hpp"
#include <cstdlib>
#include <ctime>
#include <memory>

struct Application;

struct GameManager final {
private:
    static constexpr int tile_size = 30;
    Grid m_grid;
    std::unique_ptr<Tetromino> m_active_tetromino;

public:
    GameManager() : m_grid{ Point::zero(), tile_size } {
        std::srand(std::time(nullptr));
    }

    void render(const Application& app) const;

    void spawn_tetromino() {
        static constexpr Point spawn_position{ 3, 5 };
        m_active_tetromino = std::make_unique<Tetromino>(spawn_position, get_random_tetromino_type());
    }

    void rotate_tetromino_right() {
        if (m_active_tetromino) {
            m_active_tetromino->rotate_right();
        }
    }

    void rotate_tetromino_left() {
        if (m_active_tetromino) {
            m_active_tetromino->rotate_left();
        }
    }

private:
    static TetrominoType get_random_tetromino_type() {
        return static_cast<TetrominoType>(std::rand() % (static_cast<int>(TetrominoType::LastType) + 1));
    }
};
