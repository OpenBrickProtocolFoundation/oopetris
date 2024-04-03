#pragma once

#include "bag.hpp"
#include "grid.hpp"
#include "helper/optional.hpp"
#include "helper/random.hpp"
#include "helper/types.hpp"
#include "manager/recording/tetrion_core_information.hpp"
#include "manager/service_provider.hpp"
#include "mino_stack.hpp"
#include "platform/input.hpp"
#include "tetromino.hpp"
#include "ui/layout.hpp"
#include "ui/layouts/grid_layout.hpp"
#include "ui/layouts/tile_layout.hpp"
#include "ui/widget.hpp"

#include <array>

namespace recorder {
    struct RecordingWriter;
}

enum class GameState : u8 {
    Playing,
    GameOver,
};

enum class MovementType : u8 {
    Gravity,
    Forced,
};

struct Tetrion final : public ui::Widget {
private:
    using WallKickPoint = shapes::AbstractPoint<i8>;
    using WallKickTable = std::array<std::array<WallKickPoint, 5>, 8>;
    using GridPoint = shapes::AbstractPoint<u8>;
    using ScreenCordsFunction = std::function<shapes::UPoint(const GridPoint&)>;

    static constexpr SimulationStep lock_delay = 30;
    static constexpr int num_lock_delays = 30;

    enum class RotationDirection : u8 {
        Left,
        Right,
    };

    enum class MoveDirection : u8 {
        Left,
        Right,
    };

    static constexpr u8 num_preview_tetrominos = 6;

    bool m_is_accelerated_down_movement = false;
    bool m_down_key_pressed = false;
    bool m_allowed_to_hold = true;
    bool m_is_in_lock_delay = false;
    u32 m_num_executed_lock_delays = 0;
    u64 m_next_gravity_simulation_step_index;
    u64 m_lock_delay_step_index;
    ServiceProvider* const m_service_provider;
    helper::optional<std::shared_ptr<recorder::RecordingWriter>> m_recording_writer;
    MinoStack m_mino_stack;
    Random m_random;
    u32 m_level = 0;
    u32 m_lines_cleared = 0;
    GameState m_game_state = GameState::Playing;
    int m_sequence_index = 0;
    u64 m_score = 0;
    std::array<Bag, 2> m_sequence_bags{ Bag{ m_random }, Bag{ m_random } };
    helper::optional<Tetromino> m_active_tetromino;
    helper::optional<Tetromino> m_ghost_tetromino;
    helper::optional<Tetromino> m_tetromino_on_hold;
    std::array<helper::optional<Tetromino>, num_preview_tetrominos> m_preview_tetrominos{};
    u8 m_tetrion_index;
    ui::TileLayout main_layout;


public:
    Tetrion(u8 tetrion_index,
            Random::Seed random_seed,
            u32 starting_level,
            ServiceProvider* service_provider,
            helper::optional<std::shared_ptr<recorder::RecordingWriter>> recording_writer,
            const ui::Layout& layout,
            bool is_top_level);
    void update_step(SimulationStep simulation_step_index);
    void render(const ServiceProvider& service_provider) const override;
    [[nodiscard]] Widget::EventHandleResult handle_event(const SDL_Event& event, const Window* window) override;

    // returns if the input event lead to a movement
    bool handle_input_command(InputCommand command, SimulationStep simulation_step_index);
    void spawn_next_tetromino(SimulationStep simulation_step_index);
    void spawn_next_tetromino(helper::TetrominoType type, SimulationStep simulation_step_index);
    bool rotate_tetromino_right();
    bool rotate_tetromino_left();
    bool move_tetromino_down(MovementType movement_type, SimulationStep simulation_step_index);
    bool move_tetromino_left();
    bool move_tetromino_right();
    bool drop_tetromino(SimulationStep simulation_step_index);
    void hold_tetromino(SimulationStep simulation_step_index);

    [[nodiscard]] Grid* get_grid();
    [[nodiscard]] const Grid* get_grid() const;
    [[nodiscard]] ui::GridLayout* get_text_layout();
    [[nodiscard]] const ui::GridLayout* get_text_layout() const;

    [[nodiscard]] u8 tetrion_index() const;
    [[nodiscard]] u32 level() const;
    [[nodiscard]] u64 score() const;
    [[nodiscard]] u32 lines_cleared() const;
    [[nodiscard]] const MinoStack& mino_stack() const;
    [[nodiscard]] std::unique_ptr<TetrionCoreInformation> core_information() const;

    [[nodiscard]] bool is_game_over() const;

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
    [[nodiscard]] helper::optional<const WallKickTable*> get_wall_kick_table() const;
    void reset_lock_delay(SimulationStep simulation_step_index);
    void refresh_texts();
    void clear_fully_occupied_lines();
    void lock_active_tetromino(SimulationStep simulation_step_index);
    [[nodiscard]] bool is_active_tetromino_position_valid() const;
    [[nodiscard]] bool mino_can_move_down(GridPoint position) const;
    [[nodiscard]] bool is_valid_mino_position(GridPoint position) const;

    void refresh_ghost_tetromino();
    void refresh_previews();
    helper::TetrominoType get_next_tetromino_type();

    [[nodiscard]] bool is_tetromino_position_valid(const Tetromino& tetromino) const;
    [[nodiscard]] bool tetromino_can_move_down(const Tetromino& tetromino) const;

    [[nodiscard]] u64 get_gravity_delay_frames() const;

    static u8 rotation_to_index(Rotation from, Rotation to);

    static constexpr auto wall_kick_data_jltsz = WallKickTable{
  // North -> East
        std::array{
                   WallKickPoint{ 0, 0 },
                   WallKickPoint{ -1, 0 },
                   WallKickPoint{ -1, -1 },
                   WallKickPoint{ 0, 2 },
                   WallKickPoint{ -1, 2 },
                   },
 // East -> North
        std::array{
                   WallKickPoint{ 0, 0 },
                   WallKickPoint{ 1, 0 },
                   WallKickPoint{ 1, 1 },
                   WallKickPoint{ 0, -2 },
                   WallKickPoint{ 1, -2 },
                   },
 // East -> South
        std::array{
                   WallKickPoint{ 0, 0 },
                   WallKickPoint{ 1, 0 },
                   WallKickPoint{ 1, 1 },
                   WallKickPoint{ 0, -2 },
                   WallKickPoint{ 1, -2 },
                   },
 // South -> East
        std::array{
                   WallKickPoint{ 0, 0 },
                   WallKickPoint{ -1, 0 },
                   WallKickPoint{ -1, -1 },
                   WallKickPoint{ 0, 2 },
                   WallKickPoint{ -1, 2 },
                   },
 // South -> West
        std::array{
                   WallKickPoint{ 0, 0 },
                   WallKickPoint{ 1, 0 },
                   WallKickPoint{ 1, -1 },
                   WallKickPoint{ 0, 2 },
                   WallKickPoint{ 1, 2 },
                   },
 // West -> South
        std::array{
                   WallKickPoint{ 0, 0 },
                   WallKickPoint{ -1, 0 },
                   WallKickPoint{ -1, 1 },
                   WallKickPoint{ 0, -2 },
                   WallKickPoint{ -1, -2 },
                   },
 // West -> North
        std::array{
                   WallKickPoint{ 0, 0 },
                   WallKickPoint{ -1, 0 },
                   WallKickPoint{ -1, 1 },
                   WallKickPoint{ 0, -2 },
                   WallKickPoint{ -1, -2 },
                   },
 // North -> West
        std::array{
                   WallKickPoint{ 0, 0 },
                   WallKickPoint{ 1, 0 },
                   WallKickPoint{ 1, -1 },
                   WallKickPoint{ 0, 2 },
                   WallKickPoint{ 1, 2 },
                   },
    };

    static constexpr auto wall_kick_data_i = WallKickTable{
  // North -> East
        std::array{
                   WallKickPoint{ 0, 0 },
                   WallKickPoint{ -2, 0 },
                   WallKickPoint{ 1, 0 },
                   WallKickPoint{ -2, 1 },
                   WallKickPoint{ 1, -2 },
                   },
 // East -> North
        std::array{
                   WallKickPoint{ 0, 0 },
                   WallKickPoint{ 2, 0 },
                   WallKickPoint{ -1, 0 },
                   WallKickPoint{ 2, -1 },
                   WallKickPoint{ -1, 2 },
                   },
 // East -> South
        std::array{
                   WallKickPoint{ 0, 0 },
                   WallKickPoint{ -1, 0 },
                   WallKickPoint{ 2, 0 },
                   WallKickPoint{ -1, -2 },
                   WallKickPoint{ 2, 1 },
                   },
 // South -> East
        std::array{
                   WallKickPoint{ 0, 0 },
                   WallKickPoint{ 1, 0 },
                   WallKickPoint{ -2, 0 },
                   WallKickPoint{ 1, 2 },
                   WallKickPoint{ -2, -1 },
                   },
 // South -> West
        std::array{
                   WallKickPoint{ 0, 0 },
                   WallKickPoint{ 2, 0 },
                   WallKickPoint{ -1, 0 },
                   WallKickPoint{ 2, -1 },
                   WallKickPoint{ -1, 2 },
                   },
 // West -> South
        std::array{
                   WallKickPoint{ 0, 0 },
                   WallKickPoint{ -2, 0 },
                   WallKickPoint{ 1, 0 },
                   WallKickPoint{ -2, 1 },
                   WallKickPoint{ 1, -2 },
                   },
 // West -> North
        std::array{
                   WallKickPoint{ 0, 0 },
                   WallKickPoint{ 1, 0 },
                   WallKickPoint{ -2, 0 },
                   WallKickPoint{ 1, 2 },
                   WallKickPoint{ -2, -1 },
                   },
 // North -> West
        std::array{
                   WallKickPoint{ 0, 0 },
                   WallKickPoint{ -1, 0 },
                   WallKickPoint{ 2, 0 },
                   WallKickPoint{ -1, -2 },
                   WallKickPoint{ 2, 1 },
                   },
    };

    static constexpr auto frames_per_tile = std::array<u64, 30>{ 48, 43, 38, 33, 28, 23, 18, 13, 8, 6, 5, 5, 5, 4, 4,
                                                                 4,  3,  3,  3,  2,  2,  2,  2,  2, 2, 2, 2, 2, 2, 1 };

    friend struct TetrionSnapshot;
};
