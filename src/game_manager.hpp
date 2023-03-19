#pragma once

#include "grid.hpp"
#include "tetromino.hpp"
#include <cstdlib>
#include <ctime>
#include <memory>

struct Application;

enum class GameState {
    Playing,
    GameOver,
};

struct GameManager final {
private:
    static constexpr int tile_size = 30;
    Grid m_grid;
    std::unique_ptr<Tetromino> m_active_tetromino;
    int m_level = 0;
    double m_next_gravity_step_time;
    int m_lines_cleared = 0;
    GameState m_game_state = GameState::Playing;

public:
    GameManager();
    void update();
    void render(const Application& app) const;
    void spawn_tetromino();
    void rotate_tetromino_right();
    void rotate_tetromino_left();
    void move_tetromino_down();
    void move_tetromino_left();
    void move_tetromino_right();
    void drop_tetromino();

private:
    void clear_fully_occupied_lines();
    void freeze_active_tetromino();
    bool is_active_tetromino_position_valid();
    static TetrominoType get_random_tetromino_type();

    static constexpr double get_gravity_delay(int level) {
        const int frames = (level >= frames_per_tile.size() ? frames_per_tile.back() : frames_per_tile[level]);
        return 1.0 / 60.0 * static_cast<double>(frames);
    }

    static constexpr std::array<int, 30> frames_per_tile{ 48, 43, 38, 33, 28, 23, 18, 13, 8, 6, 5, 5, 5, 4, 4,
                                                          4,  3,  3,  3,  2,  2,  2,  2,  2, 2, 2, 2, 2, 2, 1 };
};
