#pragma once

#include "bag.hpp"
#include "grid.hpp"
#include "input.hpp"
#include "tetromino.hpp"
#include "text.hpp"
#include <array>
#include <tl/optional.hpp>

struct Application;

enum class GameState {
    Playing,
    GameOver,
};

enum class MovementType {
    Gravity,
    Forced,
};

struct GameManager final {
private:
    static constexpr int tile_size = 30;

    // while holding down, this level is assumed for gravity calculation
    static constexpr int accelerated_drop_movement_level = 10;

    Grid m_grid;
    tl::optional<Tetromino> m_active_tetromino;
    tl::optional<Tetromino> m_preview_tetromino;
    int m_level = 0;
    double m_next_gravity_step_time;
    int m_lines_cleared = 0;
    GameState m_game_state = GameState::Playing;
    std::array<Bag, 2> m_sequence_bags{ Bag{}, Bag{} };
    int m_sequence_index = 0;
    std::vector<std::shared_ptr<Font>> m_fonts;
    Text m_score_text;
    int m_score = 0;
    Text m_level_text;
    Text m_cleared_lines_text;
    bool m_down_key_pressed = false;
    bool m_is_accelerated_down_movement = false;

public:
    GameManager();
    void update();
    void render(const Application& app) const;

    // returns if the input event lead to a movement
    bool handle_input_event(Input::Event event);
    void spawn_next_tetromino();
    bool rotate_tetromino_right();
    bool rotate_tetromino_left();
    bool move_tetromino_down(MovementType movement_type);
    bool move_tetromino_left();
    bool move_tetromino_right();
    bool drop_tetromino();

private:
    void refresh_texts();
    void clear_fully_occupied_lines();
    void freeze_active_tetromino();
    [[nodiscard]] bool is_active_tetromino_position_valid() const;
    [[nodiscard]] bool is_valid_mino_position(Point position) const;
    void refresh_preview();
    TetrominoType get_next_tetromino_type();

    [[nodiscard]] double get_gravity_delay() const {
        const double accelerated_gravity_delay_multiplier = (m_is_accelerated_down_movement ? 1.0 / 20.0 : 1.0);
        const int frames = (m_level >= static_cast<int>(frames_per_tile.size()) ? frames_per_tile.back() : frames_per_tile[m_level]);
        return 1.0 / 60.0 * static_cast<double>(frames) * accelerated_gravity_delay_multiplier;
    }

    static constexpr auto frames_per_tile = std::array<int, 30>{ 48, 43, 38, 33, 28, 23, 18, 13, 8, 6, 5, 5, 5, 4, 4,
                                                                 4,  3,  3,  3,  2,  2,  2,  2,  2, 2, 2, 2, 2, 2, 1 };
};
