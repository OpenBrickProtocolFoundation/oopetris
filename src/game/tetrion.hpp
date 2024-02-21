#pragma once

#include "bag.hpp"
#include "graphics/text.hpp"
#include "grid.hpp"
#include "helper/clock_source.hpp"
#include "helper/optional.hpp"
#include "helper/random.hpp"
#include "helper/types.hpp"
#include "helper/utils.hpp"
#include "input/input.hpp"
#include "manager/music_manager.hpp"
#include "manager/service_provider.hpp"
#include "mino_stack.hpp"
#include "tetromino.hpp"
#include "ui/layouts/grid_layout.hpp"
#include "ui/components/label.hpp"
#include "ui/layout.hpp"
#include "ui/layouts/tile_layout.hpp"
#include "ui/widget.hpp"

#include <array>
#include <cmath>
#include <vector>

struct RecordingWriter;

enum class GameState {
    Playing,
    GameOver,
};

enum class MovementType {
    Gravity,
    Forced,
};

struct Tetrion final : public ui::Widget {
private:
    using WallKickTable = std::array<std::array<shapes::Point, 5>, 8>;
    static constexpr SimulationStep lock_delay = 30;
    static constexpr int num_lock_delays = 30;

    enum class RotationDirection {
        Left,
        Right,
    };

    enum class MoveDirection {
        Left,
        Right,
    };

    static constexpr int num_preview_tetrominos = 6;

    bool m_is_accelerated_down_movement = false;
    bool m_down_key_pressed = false;
    bool m_allowed_to_hold = true;
    bool m_is_in_lock_delay = false;
    u32 m_num_executed_lock_delays = 0;
    u64 m_next_gravity_simulation_step_index;
    u64 m_lock_delay_step_index;
    ServiceProvider* m_service_provider;
    helpers::optional<RecordingWriter*> m_recording_writer;
    MinoStack m_mino_stack;
    Random m_random;
    u32 m_level = 0;
    u32 m_lines_cleared = 0;
    GameState m_game_state = GameState::Playing;
    int m_sequence_index = 0;
    u32 m_score = 0;
    std::array<Bag, 2> m_sequence_bags{ Bag{ m_random }, Bag{ m_random } };
    helpers::optional<Tetromino> m_active_tetromino;
    helpers::optional<Tetromino> m_ghost_tetromino;
    helpers::optional<Tetromino> m_tetromino_on_hold;
    std::array<helpers::optional<Tetromino>, num_preview_tetrominos> m_preview_tetrominos{};
    u8 m_tetrion_index;
    ui::TileLayout<2> main_layout;


public:
    Tetrion(u8 tetrion_index,
            Random::Seed random_seed,
            u32 starting_level,
            ServiceProvider* service_provider,
            helpers::optional<RecordingWriter*> recording_writer,
            const ui::Layout& layout);
    void update(SimulationStep simulation_step_index);
    void render(const ServiceProvider& service_provider) const override;
    [[nodiscard]] bool handle_event(const SDL_Event& event, const Window* window) override;

    // returns if the input event lead to a movement
    bool handle_input_command(InputCommand command, SimulationStep simulation_step_index);
    void spawn_next_tetromino(SimulationStep simulation_step_index);
    void spawn_next_tetromino(TetrominoType type, SimulationStep simulation_step_index);
    bool rotate_tetromino_right();
    bool rotate_tetromino_left();
    bool move_tetromino_down(MovementType movement_type, SimulationStep simulation_step_index);
    bool move_tetromino_left();
    bool move_tetromino_right();
    bool drop_tetromino(SimulationStep simulation_step_index);
    void hold_tetromino(SimulationStep simulation_step_index);

    [[nodiscard]] Grid* get_grid() {
        return main_layout.get<Grid>(0);
    }

    [[nodiscard]] const Grid* get_grid() const {
        return main_layout.get<Grid>(0);
    }


    [[nodiscard]] ui::GridLayout<3>* get_texts() {
        return main_layout.get<ui::GridLayout<3>>(1);
    }

    [[nodiscard]] const ui::GridLayout<3>* get_texts() const {
        return main_layout.get<ui::GridLayout<3>>(1);
    }

    [[nodiscard]] auto tetrion_index() const {
        return m_tetrion_index;
    }

    [[nodiscard]] auto level() const {
        return m_level;
    }

    [[nodiscard]] auto score() const {
        return m_score;
    }

    [[nodiscard]] auto lines_cleared() const {
        return m_lines_cleared;
    }

    [[nodiscard]] const MinoStack& mino_stack() const {
        return m_mino_stack;
    }

    [[nodiscard]] bool is_game_over() const {
        return m_game_state == GameState::GameOver;
    }

private:
    template<typename Callable>
    bool with_lock_delay(Callable movement) {
        const auto result = movement();
        if (result and m_is_in_lock_delay) {
            ++m_num_executed_lock_delays;
        }
        return result;
    }

    bool rotate(RotationDirection rotation_direction);
    bool move(MoveDirection move_direction);
    [[nodiscard]] helpers::optional<const WallKickTable*> get_wall_kick_table() const;
    void reset_lock_delay(SimulationStep simulation_step_index);
    void refresh_texts();
    void clear_fully_occupied_lines();
    void lock_active_tetromino(SimulationStep simulation_step_index);
    [[nodiscard]] bool is_active_tetromino_position_valid() const;
    [[nodiscard]] bool is_valid_mino_position(shapes::Point position) const;
    [[nodiscard]] bool is_active_tetromino_completely_visible() const;
    void refresh_ghost_tetromino();
    void refresh_previews();
    TetrominoType get_next_tetromino_type();

    [[nodiscard]] bool is_tetromino_position_valid(const Tetromino& tetromino) const;

    [[nodiscard]] u64 get_gravity_delay_frames() const {
        const auto frames = (m_level >= frames_per_tile.size() ? frames_per_tile.back() : frames_per_tile.at(m_level));
        if (m_is_accelerated_down_movement) {
            return std::max(u64{ 1 }, static_cast<u64>(std::round(static_cast<double>(frames) / 20.0)));
        }
        return frames;
    }

    static usize rotation_to_index(const Rotation from, const Rotation to) {
        if (from == Rotation::North and to == Rotation::East) {
            return 0;
        }
        if (from == Rotation::East and to == Rotation::North) {
            return 1;
        }
        if (from == Rotation::East and to == Rotation::South) {
            return 2;
        }
        if (from == Rotation::South and to == Rotation::East) {
            return 3;
        }
        if (from == Rotation::South and to == Rotation::West) {
            return 4;
        }
        if (from == Rotation::West and to == Rotation::South) {
            return 5;
        }
        if (from == Rotation::West and to == Rotation::North) {
            return 6;
        }
        if (from == Rotation::North and to == Rotation::West) {
            return 7;
        }
        utils::unreachable();
    }

    static constexpr auto wall_kick_data_jltsz = WallKickTable{
  // North -> East
        std::array{
                   shapes::Point{ 0, 0 },
                   shapes::Point{ -1, 0 },
                   shapes::Point{ -1, -1 },
                   shapes::Point{ 0, 2 },
                   shapes::Point{ -1, 2 },
                   },
 // East -> North
        std::array{
                   shapes::Point{ 0, 0 },
                   shapes::Point{ 1, 0 },
                   shapes::Point{ 1, 1 },
                   shapes::Point{ 0, -2 },
                   shapes::Point{ 1, -2 },
                   },
 // East -> South
        std::array{
                   shapes::Point{ 0, 0 },
                   shapes::Point{ 1, 0 },
                   shapes::Point{ 1, 1 },
                   shapes::Point{ 0, -2 },
                   shapes::Point{ 1, -2 },
                   },
 // South -> East
        std::array{
                   shapes::Point{ 0, 0 },
                   shapes::Point{ -1, 0 },
                   shapes::Point{ -1, -1 },
                   shapes::Point{ 0, 2 },
                   shapes::Point{ -1, 2 },
                   },
 // South -> West
        std::array{
                   shapes::Point{ 0, 0 },
                   shapes::Point{ 1, 0 },
                   shapes::Point{ 1, -1 },
                   shapes::Point{ 0, 2 },
                   shapes::Point{ 1, 2 },
                   },
 // West -> South
        std::array{
                   shapes::Point{ 0, 0 },
                   shapes::Point{ -1, 0 },
                   shapes::Point{ -1, 1 },
                   shapes::Point{ 0, -2 },
                   shapes::Point{ -1, -2 },
                   },
 // West -> North
        std::array{
                   shapes::Point{ 0, 0 },
                   shapes::Point{ -1, 0 },
                   shapes::Point{ -1, 1 },
                   shapes::Point{ 0, -2 },
                   shapes::Point{ -1, -2 },
                   },
 // North -> West
        std::array{
                   shapes::Point{ 0, 0 },
                   shapes::Point{ 1, 0 },
                   shapes::Point{ 1, -1 },
                   shapes::Point{ 0, 2 },
                   shapes::Point{ 1, 2 },
                   },
    };

    static constexpr auto wall_kick_data_i = WallKickTable{
  // North -> East
        std::array{
                   shapes::Point{ 0, 0 },
                   shapes::Point{ -2, 0 },
                   shapes::Point{ 1, 0 },
                   shapes::Point{ -2, 1 },
                   shapes::Point{ 1, -2 },
                   },
 // East -> North
        std::array{
                   shapes::Point{ 0, 0 },
                   shapes::Point{ 2, 0 },
                   shapes::Point{ -1, 0 },
                   shapes::Point{ 2, -1 },
                   shapes::Point{ -1, 2 },
                   },
 // East -> South
        std::array{
                   shapes::Point{ 0, 0 },
                   shapes::Point{ -1, 0 },
                   shapes::Point{ 2, 0 },
                   shapes::Point{ -1, -2 },
                   shapes::Point{ 2, 1 },
                   },
 // South -> East
        std::array{
                   shapes::Point{ 0, 0 },
                   shapes::Point{ 1, 0 },
                   shapes::Point{ -2, 0 },
                   shapes::Point{ 1, 2 },
                   shapes::Point{ -2, -1 },
                   },
 // South -> West
        std::array{
                   shapes::Point{ 0, 0 },
                   shapes::Point{ 2, 0 },
                   shapes::Point{ -1, 0 },
                   shapes::Point{ 2, -1 },
                   shapes::Point{ -1, 2 },
                   },
 // West -> South
        std::array{
                   shapes::Point{ 0, 0 },
                   shapes::Point{ -2, 0 },
                   shapes::Point{ 1, 0 },
                   shapes::Point{ -2, 1 },
                   shapes::Point{ 1, -2 },
                   },
 // West -> North
        std::array{
                   shapes::Point{ 0, 0 },
                   shapes::Point{ 1, 0 },
                   shapes::Point{ -2, 0 },
                   shapes::Point{ 1, 2 },
                   shapes::Point{ -2, -1 },
                   },
 // North -> West
        std::array{
                   shapes::Point{ 0, 0 },
                   shapes::Point{ -1, 0 },
                   shapes::Point{ 2, 0 },
                   shapes::Point{ -1, -2 },
                   shapes::Point{ 2, 1 },
                   },
    };

    static constexpr auto frames_per_tile = std::array<u64, 30>{ 48, 43, 38, 33, 28, 23, 18, 13, 8, 6, 5, 5, 5, 4, 4,
                                                                 4,  3,  3,  3,  2,  2,  2,  2,  2, 2, 2, 2, 2, 2, 1 };

    friend struct TetrionSnapshot;
};
