#include <core/helper/magic_enum_wrapper.hpp>
#include <core/helper/utils.hpp>
#include <recordings/utility/recording_writer.hpp>

#include "helper/constants.hpp"
#include "helper/graphic_utils.hpp"
#include "helper/music_utils.hpp"
#include "manager/music_manager.hpp"
#include "simulated_tetrion.hpp"

#include <cassert>
#include <spdlog/spdlog.h>


SimulatedTetrion::SimulatedTetrion(
        const u8 tetrion_index,
        const Random::Seed random_seed,
        const u32 starting_level,
        ServiceProvider* const service_provider,
        std::optional<std::shared_ptr<recorder::RecordingWriter>> recording_writer
)
    : m_lock_delay_step_index{ lock_delay },
      m_level{ starting_level },
      m_random{ random_seed },
      m_tetrion_index{ tetrion_index },
      m_next_gravity_simulation_step_index{ get_gravity_delay_frames() },
      m_recording_writer{ std::move(recording_writer) },
      m_service_provider{ service_provider } { }

SimulatedTetrion::~SimulatedTetrion() = default;

void SimulatedTetrion::update_step(const SimulationStep simulation_step_index) {
    switch (m_game_state) {
        case GameState::Playing: {
            if (simulation_step_index >= m_next_gravity_simulation_step_index) {
                assert(simulation_step_index == m_next_gravity_simulation_step_index and "frame skipped?!");
                if (m_is_accelerated_down_movement and not m_down_key_pressed) {
                    assert(m_next_gravity_simulation_step_index >= get_gravity_delay_frames() and "overflow");
                    m_next_gravity_simulation_step_index -= get_gravity_delay_frames();
                    m_is_accelerated_down_movement = false;
                } else {
                    if (move_tetromino_down(
                                m_is_accelerated_down_movement ? MovementType::Forced : MovementType::Gravity,
                                simulation_step_index
                        )) {
                        reset_lock_delay(simulation_step_index);
                    }
                }
                m_next_gravity_simulation_step_index += get_gravity_delay_frames();
            }

            refresh_ghost_tetromino();
            break;
        }
        case GameState::GameOver:
        default:
            break;
    }
}

bool SimulatedTetrion::handle_input_command(
        const input::GameInputCommand command,
        const SimulationStep simulation_step_index
) {
    switch (command) {
        case input::GameInputCommand::RotateLeft:
            if (rotate_tetromino_left()) {
                reset_lock_delay(simulation_step_index);
                return true;
            }
            return false;
        case input::GameInputCommand::RotateRight:
            if (rotate_tetromino_right()) {
                reset_lock_delay(simulation_step_index);
                return true;
            }
            return false;
        case input::GameInputCommand::MoveLeft:
            if (move_tetromino_left()) {
                reset_lock_delay(simulation_step_index);
                return true;
            }
            return false;
        case input::GameInputCommand::MoveRight:
            if (move_tetromino_right()) {
                reset_lock_delay(simulation_step_index);
                return true;
            }
            return false;
        case input::GameInputCommand::MoveDown:
            //TODO(Totto): use input_type() != InputType:Touch
#if not defined(__ANDROID__)
            m_down_key_pressed = true;
            m_is_accelerated_down_movement = true;
            m_next_gravity_simulation_step_index = simulation_step_index + get_gravity_delay_frames();
#endif
            if (move_tetromino_down(MovementType::Forced, simulation_step_index)) {
                reset_lock_delay(simulation_step_index);
                return true;
            }
            return false;
        case input::GameInputCommand::Drop:
            m_lock_delay_step_index = simulation_step_index; // lock instantly
            return drop_tetromino(simulation_step_index);
        case input::GameInputCommand::ReleaseMoveDown: {
            m_down_key_pressed = false;
            return false;
        }
        case input::GameInputCommand::Hold:
            if (m_allowed_to_hold) {
                hold_tetromino(simulation_step_index);
                reset_lock_delay(simulation_step_index);
                m_allowed_to_hold = false;
                return true;
            }
            return false;
        default:
            assert(false and "unknown GameInput");
            return false;
    }
}

void SimulatedTetrion::spawn_next_tetromino(const SimulationStep simulation_step_index) {
    spawn_next_tetromino(get_next_tetromino_type(), simulation_step_index);
}

void SimulatedTetrion::spawn_next_tetromino(
        const helper::TetrominoType type,
        const SimulationStep simulation_step_index
) {
    constexpr GridPoint spawn_position{ 3, 0 };
    m_active_tetromino = Tetromino{ spawn_position, type };
    refresh_previews();
    if (not is_active_tetromino_position_valid()) {
        m_game_state = GameState::GameOver;

        auto current_pieces = m_active_tetromino.value().minos();

        bool all_valid{ false };
        u8 move_up = 0;
        while (not all_valid) {
            all_valid = true;
            for (auto& mino : current_pieces) {
                if (mino.position().y != 0) {
                    mino.position() = mino.position() - GridPoint{ 0, 1 };
                    if (not is_valid_mino_position(mino.position())) {
                        all_valid = false;
                    }
                }
            }

            ++move_up;
        }

        for (const Mino& mino : m_active_tetromino->minos()) {
            auto position = mino.position();
            if (mino.position().y >= move_up && move_up != 0) {
                position -= GridPoint{ 0, move_up };
                m_mino_stack.set(position, mino.type());
            }
        }

        spdlog::info("game over");
        if (m_recording_writer.has_value()) {
            spdlog::info("writing snapshot");
            std::ignore = m_recording_writer.value()->add_snapshot(simulation_step_index, core_information());
        }
        m_active_tetromino = {};
        m_ghost_tetromino = {};
        return;
    }

    m_next_gravity_simulation_step_index = simulation_step_index + get_gravity_delay_frames();
    refresh_ghost_tetromino();
}

bool SimulatedTetrion::rotate_tetromino_right() {
    return with_lock_delay([&]() { return rotate(RotationDirection::Right); });
}

bool SimulatedTetrion::rotate_tetromino_left() {
    return with_lock_delay([&]() { return rotate(RotationDirection::Left); });
}

bool SimulatedTetrion::move_tetromino_down(MovementType movement_type, const SimulationStep simulation_step_index) {
    if (not m_active_tetromino.has_value()) {
        return false;
    }
    if (movement_type == MovementType::Forced) {
        m_score += 4;
    }


    if (tetromino_can_move_down(m_active_tetromino.value())) {
        m_active_tetromino->move_down();
        return true;
    }

    m_is_in_lock_delay = true;
    if ((m_is_in_lock_delay and m_num_executed_lock_delays >= num_lock_delays)
        or simulation_step_index >= m_lock_delay_step_index) {
        lock_active_tetromino(simulation_step_index);
        reset_lock_delay(simulation_step_index);
    } else {
        m_next_gravity_simulation_step_index = simulation_step_index + 1;
    }
    return false;
}

bool SimulatedTetrion::move_tetromino_left() {
    return with_lock_delay([&]() { return move(MoveDirection::Left); });
}

bool SimulatedTetrion::move_tetromino_right() {
    return with_lock_delay([&]() { return move(MoveDirection::Right); });
}

bool SimulatedTetrion::drop_tetromino(const SimulationStep simulation_step_index) {
    if (not m_active_tetromino.has_value()) {
        return false;
    }
    u64 num_movements = 0;
    while (tetromino_can_move_down(m_active_tetromino.value())) {
        ++num_movements;
        m_active_tetromino->move_down();
    }

    m_score += static_cast<u64>(4) * num_movements;
    lock_active_tetromino(simulation_step_index);
    return num_movements > 0;
}

void SimulatedTetrion::hold_tetromino(const SimulationStep simulation_step_index) {
    if (not m_active_tetromino.has_value()) {
        return;
    }

    if (not m_tetromino_on_hold.has_value()) {
        m_tetromino_on_hold = Tetromino{ grid::hold_tetromino_position, m_active_tetromino->type() };
        spawn_next_tetromino(simulation_step_index);
    } else {
        const auto on_hold = m_tetromino_on_hold->type();
        m_tetromino_on_hold = Tetromino{ grid::hold_tetromino_position, m_active_tetromino->type() };
        spawn_next_tetromino(on_hold, simulation_step_index);
    }
}

[[nodiscard]] u8 SimulatedTetrion::tetrion_index() const {
    return m_tetrion_index;
}

[[nodiscard]] u32 SimulatedTetrion::level() const {
    return m_level;
}

[[nodiscard]] u64 SimulatedTetrion::score() const {
    return m_score;
}

[[nodiscard]] u32 SimulatedTetrion::lines_cleared() const {
    return m_lines_cleared;
}

[[nodiscard]] const MinoStack& SimulatedTetrion::mino_stack() const {
    return m_mino_stack;
}

[[nodiscard]] std::unique_ptr<TetrionCoreInformation> SimulatedTetrion::core_information() const {

    return std::make_unique<TetrionCoreInformation>(m_tetrion_index, m_level, m_score, m_lines_cleared, m_mino_stack);
}

[[nodiscard]] bool SimulatedTetrion::is_game_over() const {
    return m_game_state == GameState::GameOver;
}

void SimulatedTetrion::reset_lock_delay(const SimulationStep simulation_step_index) {
    m_lock_delay_step_index = simulation_step_index + lock_delay;
}

void SimulatedTetrion::refresh_texts() { }

void SimulatedTetrion::clear_fully_occupied_lines() {
    bool cleared = false;
    const u32 lines_cleared_before = m_lines_cleared;
    do { // NOLINT(cppcoreguidelines-avoid-do-while)
        cleared = false;
        for (u8 row = 0; row < grid::height_in_tiles; ++row) {
            bool fully_occupied = true;
            for (u8 column = 0; column < grid::width_in_tiles; ++column) {
                if (m_mino_stack.is_empty(GridPoint{ column, row })) {
                    fully_occupied = false;
                    break;
                }
            }

            if (fully_occupied) {
                ++m_lines_cleared;
                const auto level = m_lines_cleared / 10;
                if (level > m_level) {
                    m_level = level;
                    spdlog::info("new level: {}", m_level);
                    if (level == constants::music_change_level) {
                        if (m_service_provider != nullptr) {
                            m_service_provider->music_manager()
                                    .load_and_play_music(
                                            utils::get_assets_folder() / "music"
                                            / utils::get_supported_music_extension("03. Game Theme (50 Left)")
                                    )
                                    .and_then(utils::log_error);
                        }
                    }
                }
                m_mino_stack.clear_row_and_let_sink(static_cast<int>(row));
                cleared = true;
                break;
            }
        }
    } while (cleared);
    const u32 num_lines_cleared = m_lines_cleared - lines_cleared_before;
    static constexpr std::array<u32, 5> score_per_line_multiplier{ 0, 40, 100, 300, 1200 };
    m_score += static_cast<u64>(score_per_line_multiplier.at(num_lines_cleared)) * static_cast<u64>(m_level + 1);
}

void SimulatedTetrion::lock_active_tetromino(const SimulationStep simulation_step_index) {
    assert(m_active_tetromino.has_value());
    for (const Mino& mino : m_active_tetromino->minos()) {
        m_mino_stack.set(mino.position(), mino.type());
    }
    m_allowed_to_hold = true;
    m_is_in_lock_delay = false;
    m_num_executed_lock_delays = 0;
    clear_fully_occupied_lines();
    spawn_next_tetromino(simulation_step_index);
    refresh_texts();
    reset_lock_delay(simulation_step_index);

    // save a snapshot on every freeze (only in debug builds)
#if !defined(NDEBUG)
    if (m_recording_writer) {
        spdlog::debug("adding snapshot at step {}", simulation_step_index);
        std::ignore = (*m_recording_writer)->add_snapshot(simulation_step_index, core_information());
    }
#endif
}

bool SimulatedTetrion::is_active_tetromino_position_valid() const {
    if (not m_active_tetromino) {
        return false;
    }
    return is_tetromino_position_valid(m_active_tetromino.value());
}

bool SimulatedTetrion::is_valid_mino_position(GridPoint position) const {
    return position.x < grid::width_in_tiles and position.y < grid::height_in_tiles and m_mino_stack.is_empty(position);
}

bool SimulatedTetrion::mino_can_move_down(GridPoint position) const {
    if (position.y == (grid::height_in_tiles - 1)) {
        return false;
    }

    return is_valid_mino_position(position + GridPoint{ 0, 1 });
}


void SimulatedTetrion::refresh_ghost_tetromino() {
    if (not m_active_tetromino.has_value()) {
        m_ghost_tetromino = {};
        return;
    }
    m_ghost_tetromino = m_active_tetromino.value();
    while (tetromino_can_move_down(m_ghost_tetromino.value())) {
        m_ghost_tetromino->move_down();
    }
}

void SimulatedTetrion::refresh_previews() {
    auto sequence_index = m_sequence_index;
    auto bag_index = usize{ 0 };
    for (std::remove_cvref_t<decltype(num_preview_tetrominos)> i = 0; i < num_preview_tetrominos; ++i) {
        m_preview_tetrominos.at(static_cast<usize>(i)) = Tetromino{
            grid::preview_tetromino_position + shapes::UPoint{ 0, static_cast<u32>(grid::preview_padding * i) },
            m_sequence_bags.at(bag_index)[sequence_index]
        };
        ++sequence_index;
        static constexpr auto bag_size = decltype(m_sequence_bags)::value_type::size();
        if (sequence_index >= bag_size) {
            assert(sequence_index == bag_size);
            sequence_index = 0;
            ++bag_index;
            assert(bag_index < m_sequence_bags.size());
        }
    }
}

helper::TetrominoType SimulatedTetrion::get_next_tetromino_type() {
    const helper::TetrominoType next_type = m_sequence_bags[0][m_sequence_index];
    m_sequence_index = (m_sequence_index + 1) % Bag::size();
    if (m_sequence_index == 0) {
        // we had a wrap-around
        m_sequence_bags[0] = m_sequence_bags[1];
        m_sequence_bags[1] = Bag{ m_random };
    }
    return next_type;
}

bool SimulatedTetrion::tetromino_can_move_down(const Tetromino& tetromino) const {
    return not std::ranges::any_of(tetromino.minos(), [this](const Mino& mino) {
        return not mino_can_move_down(mino.position());
    });
}


[[nodiscard]] u64 SimulatedTetrion::get_gravity_delay_frames() const {
    const auto frames = (m_level >= frames_per_tile.size() ? frames_per_tile.back() : frames_per_tile.at(m_level));
    if (m_is_accelerated_down_movement) {
        return std::max(u64{ 1 }, static_cast<u64>(std::round(static_cast<double>(frames) / 20.0)));
    }
    return frames;
}

u8 SimulatedTetrion::rotation_to_index(const Rotation from, const Rotation rotation_to) {
    if (from == Rotation::North and rotation_to == Rotation::East) {
        return 0;
    }
    if (from == Rotation::East and rotation_to == Rotation::North) {
        return 1;
    }
    if (from == Rotation::East and rotation_to == Rotation::South) {
        return 2;
    }
    if (from == Rotation::South and rotation_to == Rotation::East) {
        return 3;
    }
    if (from == Rotation::South and rotation_to == Rotation::West) {
        return 4;
    }
    if (from == Rotation::West and rotation_to == Rotation::South) {
        return 5;
    }
    if (from == Rotation::West and rotation_to == Rotation::North) {
        return 6;
    }
    if (from == Rotation::North and rotation_to == Rotation::West) {
        return 7;
    }
    UNREACHABLE();
}

bool SimulatedTetrion::is_tetromino_position_valid(const Tetromino& tetromino) const {
    return not std::ranges::any_of(tetromino.minos(), [this](const Mino& mino) {
        return not is_valid_mino_position(mino.position());
    });
}

bool SimulatedTetrion::rotate(SimulatedTetrion::RotationDirection rotation_direction) {
    if (not m_active_tetromino) {
        return false;
    }

    const auto wall_kick_table = get_wall_kick_table();
    if (not wall_kick_table.has_value()) {
        return false;
    }

    const auto from_rotation = m_active_tetromino->rotation();
    const auto to_rotation = from_rotation + static_cast<i8>(rotation_direction == RotationDirection::Left ? -1 : 1);
    const auto table_index = rotation_to_index(from_rotation, to_rotation);

    if (rotation_direction == RotationDirection::Left) {
        m_active_tetromino->rotate_left();
    } else {
        m_active_tetromino->rotate_right();
    }

    for (const auto& translation : (*wall_kick_table)->at(table_index)) {
        m_active_tetromino->move(translation);
        if (is_active_tetromino_position_valid()) {
            return true;
        }
        m_active_tetromino->move(-translation);
    }

    if (rotation_direction == RotationDirection::Left) {
        m_active_tetromino->rotate_right();
    } else {
        m_active_tetromino->rotate_left();
    }
    return false;
}

bool SimulatedTetrion::move(const SimulatedTetrion::MoveDirection move_direction) {
    if (not m_active_tetromino) {
        return false;
    }

    switch (move_direction) {
        case MoveDirection::Left:
            m_active_tetromino->move_left();
            if (not is_active_tetromino_position_valid()) {
                m_active_tetromino->move_right();
                return false;
            }
            return true;
        case MoveDirection::Right:
            m_active_tetromino->move_right();
            if (not is_active_tetromino_position_valid()) {
                m_active_tetromino->move_left();
                return false;
            }
            return true;
    }

    UNREACHABLE();
}

std::optional<const SimulatedTetrion::WallKickTable*> SimulatedTetrion::get_wall_kick_table() const {
    assert(m_active_tetromino.has_value() and "no active tetromino");
    const auto type = m_active_tetromino->type(); // NOLINT(bugprone-unchecked-optional-access)
    switch (type) {
        case helper::TetrominoType::J:
        case helper::TetrominoType::L:
        case helper::TetrominoType::T:
        case helper::TetrominoType::S:
        case helper::TetrominoType::Z:
            return &wall_kick_data_jltsz;
        case helper::TetrominoType::I:
            return &wall_kick_data_i;
        case helper::TetrominoType::O:
            return {};
        default:
            UNREACHABLE();
    }
}
