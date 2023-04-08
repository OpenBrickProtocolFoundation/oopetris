#pragma once

#include "bag.hpp"
#include "grid.hpp"
#include "input.hpp"
#include "random.hpp"
#include "tetromino.hpp"
#include "text.hpp"
#include "types.hpp"
#include <array>
#include <cmath>
#include <tl/optional.hpp>
#include <vector>

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
    static constexpr u64 lock_delay = 30;

    // while holding down, this level is assumed for gravity calculation
    static constexpr int accelerated_drop_movement_level = 10;

    Random m_random;
    Grid m_grid;
    tl::optional<Tetromino> m_active_tetromino;
    tl::optional<Tetromino> m_ghost_tetromino;
    tl::optional<Tetromino> m_preview_tetromino;
    tl::optional<Tetromino> m_tetromino_on_hold;
    int m_level = 0;
    u64 m_next_gravity_simulation_step_index;
    int m_lines_cleared = 0;
    GameState m_game_state = GameState::Playing;
    std::array<Bag, 2> m_sequence_bags{ Bag{ m_random }, Bag{ m_random } };
    int m_sequence_index = 0;
    std::vector<std::shared_ptr<Font>> m_fonts;
    Text m_score_text;
    int m_score = 0;
    Text m_level_text;
    Text m_cleared_lines_text;
    bool m_down_key_pressed = false;
    bool m_is_accelerated_down_movement = false;
    Recording m_recording;
    bool m_record_game;
    bool m_allowed_to_hold = true;
    u64 m_lock_delay_step_index;

public:
    GameManager(Random::Seed random_seed, bool record_game);
    void update();
    void render(const Application& app) const;

    // returns if the input event lead to a movement
    bool handle_input_event(InputEvent event);
    void spawn_next_tetromino();
    void spawn_next_tetromino(TetrominoType type);
    bool rotate_tetromino_right();
    bool rotate_tetromino_left();
    bool move_tetromino_down(MovementType movement_type);
    bool move_tetromino_left();
    bool move_tetromino_right();
    bool drop_tetromino();
    void hold_tetromino();

private:
    void reset_lock_delay();
    void refresh_texts();
    void clear_fully_occupied_lines();
    void lock_active_tetromino();
    [[nodiscard]] bool is_active_tetromino_position_valid() const;
    [[nodiscard]] bool is_valid_mino_position(Point position) const;
    [[nodiscard]] bool is_active_tetromino_completely_visible() const;
    void refresh_ghost_tetromino();
    void refresh_preview();
    TetrominoType get_next_tetromino_type();

    [[nodiscard]] bool is_tetromino_position_valid(const Tetromino& tetromino) const;

    [[nodiscard]] u64 get_gravity_delay_frames() const {
        const auto frames =
                (m_level >= static_cast<int>(frames_per_tile.size()) ? frames_per_tile.back() : frames_per_tile[m_level]
                );
        if (m_is_accelerated_down_movement) {
            return std::max(u64{ 1 }, static_cast<u64>(std::round(static_cast<double>(frames) / 20.0)));
        }
        return frames;
    }

    void save_recording() const;

    static constexpr auto frames_per_tile = std::array<u64, 30>{ 48, 43, 38, 33, 28, 23, 18, 13, 8, 6, 5, 5, 5, 4, 4,
                                                                 4,  3,  3,  3,  2,  2,  2,  2,  2, 2, 2, 2, 2, 2, 1 };
};
