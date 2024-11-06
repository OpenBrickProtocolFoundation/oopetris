#pragma once

#include <core/game/mino_stack.hpp>
#include <core/helper/random.hpp>
#include <core/helper/types.hpp>
#include <recordings/utility/recording_writer.hpp>
#include <recordings/utility/tetrion_core_information.hpp>

#include "bag.hpp"
#include "grid.hpp"
#include "helper/windows.hpp"
#include "input/game_input.hpp"
#include "manager/service_provider.hpp"
#include "tetromino.hpp"
#include "ui/layouts/grid_layout.hpp"
#include "ui/widget.hpp"

#include <array>


enum class GameState : u8 {
    Playing,
    GameOver,
};

enum class MovementType : u8 {
    Gravity,
    Forced,
};

struct SimulatedTetrion {
private:
    using WallKickPoint = shapes::AbstractPoint<i8>;
    using WallKickTable = std::array<std::array<WallKickPoint, 5>, 8>;

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
    u64 m_lock_delay_step_index;

protected:
    MinoStack
            m_mino_stack; // NOLINT(misc-non-private-member-variables-in-classes,cppcoreguidelines-non-private-member-variables-in-classes)
    u32 m_level; // NOLINT(misc-non-private-member-variables-in-classes,cppcoreguidelines-non-private-member-variables-in-classes)
    u32 m_lines_cleared = // NOLINT(misc-non-private-member-variables-in-classes,cppcoreguidelines-non-private-member-variables-in-classes)
            0;
    u64 m_score = // NOLINT(misc-non-private-member-variables-in-classes,cppcoreguidelines-non-private-member-variables-in-classes)
            0;


    std::optional<Tetromino>
            m_active_tetromino; // NOLINT(misc-non-private-member-variables-in-classes,cppcoreguidelines-non-private-member-variables-in-classes)
    std::optional<Tetromino>
            m_ghost_tetromino; // NOLINT(misc-non-private-member-variables-in-classes,cppcoreguidelines-non-private-member-variables-in-classes)
    std::optional<Tetromino>
            m_tetromino_on_hold; // NOLINT(misc-non-private-member-variables-in-classes,cppcoreguidelines-non-private-member-variables-in-classes)
    std::array<std::optional<Tetromino>, num_preview_tetrominos>
            m_preview_tetrominos{}; // NOLINT(misc-non-private-member-variables-in-classes,cppcoreguidelines-non-private-member-variables-in-classes)

private:
    Random m_random;
    GameState m_game_state = GameState::Playing;
    int m_sequence_index = 0;
    std::array<Bag, 2> m_sequence_bags{ Bag{ m_random }, Bag{ m_random } };
    u8 m_tetrion_index;
    u64 m_next_gravity_simulation_step_index;
    std::optional<std::shared_ptr<recorder::RecordingWriter>> m_recording_writer;

protected:
    ServiceProvider*
            m_service_provider; // NOLINT(misc-non-private-member-variables-in-classes,cppcoreguidelines-non-private-member-variables-in-classes)

public:
    OOPETRIS_GRAPHICS_EXPORTED SimulatedTetrion(
            u8 tetrion_index,
            Random::Seed random_seed,
            u32 starting_level,
            ServiceProvider* service_provider,
            std::optional<std::shared_ptr<recorder::RecordingWriter>> recording_writer
    );

    OOPETRIS_GRAPHICS_EXPORTED virtual ~SimulatedTetrion();

    OOPETRIS_GRAPHICS_EXPORTED SimulatedTetrion(const SimulatedTetrion& other);
    OOPETRIS_GRAPHICS_EXPORTED SimulatedTetrion& operator=(const SimulatedTetrion& other) = delete;

    OOPETRIS_GRAPHICS_EXPORTED SimulatedTetrion(SimulatedTetrion&& other) noexcept;
    OOPETRIS_GRAPHICS_EXPORTED SimulatedTetrion& operator=(SimulatedTetrion&& other) noexcept = delete;

    OOPETRIS_GRAPHICS_EXPORTED void update_step(SimulationStep simulation_step_index);

    // returns if the input event lead to a movement
    OOPETRIS_GRAPHICS_EXPORTED bool
    handle_input_command(input::GameInputCommand command, SimulationStep simulation_step_index);
    OOPETRIS_GRAPHICS_EXPORTED void spawn_next_tetromino(SimulationStep simulation_step_index);
    OOPETRIS_GRAPHICS_EXPORTED void
    spawn_next_tetromino(helper::TetrominoType type, SimulationStep simulation_step_index);
    OOPETRIS_GRAPHICS_EXPORTED bool rotate_tetromino_right();
    OOPETRIS_GRAPHICS_EXPORTED bool rotate_tetromino_left();
    OOPETRIS_GRAPHICS_EXPORTED bool
    move_tetromino_down(MovementType movement_type, SimulationStep simulation_step_index);
    OOPETRIS_GRAPHICS_EXPORTED bool move_tetromino_left();
    OOPETRIS_GRAPHICS_EXPORTED bool move_tetromino_right();
    OOPETRIS_GRAPHICS_EXPORTED bool drop_tetromino(SimulationStep simulation_step_index);
    OOPETRIS_GRAPHICS_EXPORTED void hold_tetromino(SimulationStep simulation_step_index);

    OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] u8 tetrion_index() const;
    OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] u32 level() const;
    OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] u64 score() const;
    OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] u32 lines_cleared() const;
    OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] const MinoStack& mino_stack() const;
    OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] std::unique_ptr<TetrionCoreInformation> core_information() const;

    OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] bool is_game_over() const;

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
    [[nodiscard]] std::optional<const WallKickTable*> get_wall_kick_table() const;
    void reset_lock_delay(SimulationStep simulation_step_index);
    virtual void refresh_texts();
    void clear_fully_occupied_lines();
    void lock_active_tetromino(SimulationStep simulation_step_index);
    [[nodiscard]] bool is_active_tetromino_position_valid() const;
    [[nodiscard]] bool mino_can_move_down(grid::GridPoint position) const;
    [[nodiscard]] bool is_valid_mino_position(grid::GridPoint position) const;

    void refresh_ghost_tetromino();
    void refresh_previews();
    helper::TetrominoType get_next_tetromino_type();

    [[nodiscard]] bool is_tetromino_position_valid(const Tetromino& tetromino) const;
    [[nodiscard]] bool tetromino_can_move_down(const Tetromino& tetromino) const;

    [[nodiscard]] u64 get_gravity_delay_frames() const;

    static u8 rotation_to_index(Rotation from, Rotation rotation_to);

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
