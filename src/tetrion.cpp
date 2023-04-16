#include "tetrion.hpp"
#include "application.hpp"
#include "recording.hpp"
#include <cassert>
#include <fstream>
#include <spdlog/spdlog.h>
#include <sstream>

Tetrion::Tetrion(
        const u8 tetrion_index,
        const Random::Seed random_seed,
        const int starting_level,
        tl::optional<RecordingWriter*> recording_writer,
        const bool use_player_text
)
    : m_tetrion_index{ tetrion_index },
      m_random{ random_seed },
      m_grid{Point{
        static_cast<int>((tetrion_index * Tetrion::size_per_field) + (tetrion_index * Tetrion::space_between)), 0
    }, tile_size },
      m_level{ starting_level },
      m_next_gravity_simulation_step_index{ get_gravity_delay_frames() },
      m_recording_writer{ recording_writer },
      m_lock_delay_step_index{ Application::simulation_step_index() + lock_delay }, m_use_player_text{use_player_text}  {

#if defined(__ANDROID__)
    constexpr auto font_path = "fonts/PressStart2P.ttf";
    constexpr auto font_size = 35;
#else
    constexpr auto font_path = "assets/fonts/PressStart2P.ttf";
    constexpr auto font_size = 18;
#endif
    m_fonts.push_back(std::make_shared<Font>(font_path, font_size));
    m_text_rows.emplace_back(
            Point{ m_grid.to_screen_coords(Grid::preview_tetromino_position + Point{ 0, Grid::preview_extends.y }) },
            Color::white(), "score: 0", m_fonts.front()
    );
    m_text_rows.emplace_back(
            Point{ m_grid.to_screen_coords(
                    Grid::preview_tetromino_position + Point{ 0, Grid::preview_extends.y + 1 }
            ) },
            Color::white(), "level: 0", m_fonts.front()
    );
    m_text_rows.emplace_back(
            Point{ m_grid.to_screen_coords(
                    Grid::preview_tetromino_position + Point{ 0, Grid::preview_extends.y + 2 }
            ) },
            Color::white(), "lines: 0", m_fonts.front()
    );
    if (m_use_player_text) {
        m_text_rows.emplace_back(
                Point{ m_grid.to_screen_coords(
                        Grid::preview_tetromino_position + Point{ 0, Grid::preview_extends.y + 3 }
                ) },
                Color::white(), "player: 0", m_fonts.front()
        );
    }
    refresh_texts();
}

void Tetrion::update() {
    switch (m_game_state) {
        case GameState::Playing: {
            if (Application::simulation_step_index() >= m_next_gravity_simulation_step_index) {
                assert(Application::simulation_step_index() == m_next_gravity_simulation_step_index
                       and "frame skipped?!");
                if (m_is_accelerated_down_movement and not m_down_key_pressed) {
                    assert(m_next_gravity_simulation_step_index >= get_gravity_delay_frames() and "overflow");
                    m_next_gravity_simulation_step_index -= get_gravity_delay_frames();
                    m_is_accelerated_down_movement = false;
                } else {
                    if (move_tetromino_down(
                                m_is_accelerated_down_movement ? MovementType::Forced : MovementType::Gravity
                        )) {
                        reset_lock_delay();
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

void Tetrion::render(const Application& app) const {
    m_grid.render(app);
    m_mino_stack.draw_minos(app, m_grid);
    if (m_active_tetromino) {
        m_active_tetromino->render(app, m_grid);
    }
    if (m_ghost_tetromino) {
        m_ghost_tetromino->render(app, m_grid, true);
    }
    if (m_preview_tetromino) {
        m_preview_tetromino->render(app, m_grid);
    }

    if (m_tetromino_on_hold) {
        m_tetromino_on_hold->render(app, m_grid);
    }

    for (const auto& text : m_text_rows) {
        text.render(app);
    }
}

bool Tetrion::handle_input_command(const InputCommand command) {
    switch (command) {
        case InputCommand::RotateLeft:
            if (rotate_tetromino_left()) {
                reset_lock_delay();
                return true;
            }
            return false;
        case InputCommand::RotateRight:
            if (rotate_tetromino_right()) {
                reset_lock_delay();
                return true;
            }
            return false;
        case InputCommand::MoveLeft:
            if (move_tetromino_left()) {
                reset_lock_delay();
                return true;
            }
            return false;
        case InputCommand::MoveRight:
            if (move_tetromino_right()) {
                reset_lock_delay();
                return true;
            }
            return false;
        case InputCommand::MoveDown:
#if not defined(__ANDROID__)
            m_down_key_pressed = true;
            m_is_accelerated_down_movement = true;
            m_next_gravity_simulation_step_index = Application::simulation_step_index() + get_gravity_delay_frames();
#endif
            if (move_tetromino_down(MovementType::Forced)) {
                reset_lock_delay();
                return true;
            }
            return false;
        case InputCommand::Drop:
            m_lock_delay_step_index = Application::simulation_step_index(); // lock instantly
            return drop_tetromino();
        case InputCommand::ReleaseMoveDown: {
            m_down_key_pressed = false;
            return false;
        }
        case InputCommand::Hold:
            if (m_allowed_to_hold) {
                hold_tetromino();
                reset_lock_delay();
                m_allowed_to_hold = false;
                return true;
            }
            return false;
        default:
            assert(false and "unknown event");
            return false;
    }
}

void Tetrion::spawn_next_tetromino() {
    spawn_next_tetromino(get_next_tetromino_type());
}

void Tetrion::spawn_next_tetromino(const TetrominoType type) {
    static constexpr Point spawn_position{ 3, 0 };
    m_active_tetromino = Tetromino{ spawn_position, type };
    refresh_preview();
    if (not is_active_tetromino_position_valid()) {
        m_game_state = GameState::GameOver;
        spdlog::info("game over");
        if (m_recording_writer.has_value()) {
            spdlog::info("writing snapshot");
            (*m_recording_writer)->add_snapshot(m_tetrion_index, Application::simulation_step_index(), *this);
        }
        m_active_tetromino = {};
        return;
    }
    for (int i = 0; not is_active_tetromino_completely_visible() and i < Grid::invisible_rows; ++i) {
        m_active_tetromino->move_down();
        if (not is_active_tetromino_position_valid()) {
            m_active_tetromino->move_up();
            break;
        }
    }
    m_next_gravity_simulation_step_index = Application::simulation_step_index() + get_gravity_delay_frames();
    refresh_ghost_tetromino();
}

bool Tetrion::rotate_tetromino_right() {
    return with_lock_delay([&]() { return rotate(RotationDirection::Right); });
}

bool Tetrion::rotate_tetromino_left() {
    return with_lock_delay([&]() { return rotate(RotationDirection::Left); });
}

bool Tetrion::move_tetromino_down(MovementType movement_type) {
    if (not m_active_tetromino) {
        return false;
    }
    if (movement_type == MovementType::Forced) {
        m_score += 4;
    }

    m_active_tetromino->move_down();
    if (not is_active_tetromino_position_valid()) {
        m_is_in_lock_delay = true;
        m_active_tetromino->move_up();
        if ((m_is_in_lock_delay and m_num_executed_lock_delays >= num_lock_delays)
            or Application::simulation_step_index() >= m_lock_delay_step_index) {
            lock_active_tetromino();
            reset_lock_delay();
        } else {
            m_next_gravity_simulation_step_index = Application::simulation_step_index() + 1;
        }
        return false;
    }
    return true;
}

bool Tetrion::move_tetromino_left() {
    return with_lock_delay([&]() { return move(MoveDirection::Left); });
}

bool Tetrion::move_tetromino_right() {
    return with_lock_delay([&]() { return move(MoveDirection::Right); });
}

bool Tetrion::drop_tetromino() {
    if (not m_active_tetromino) {
        return false;
    }
    int num_movements = 0;
    while (is_active_tetromino_position_valid()) {
        ++num_movements;
        m_active_tetromino->move_down();
    }
    m_active_tetromino->move_up();
    m_score += 4 * num_movements;
    lock_active_tetromino();
    return num_movements > 0;
}


void Tetrion::set_player_num(std::size_t player_num) {
    m_player_num = player_num;
    refresh_texts();
}

void Tetrion::hold_tetromino() {
    if (not m_active_tetromino.has_value()) {
        return;
    }

    if (not m_tetromino_on_hold.has_value()) {
        m_tetromino_on_hold = Tetromino{ Grid::hold_tetromino_position, m_active_tetromino->type() };
        spawn_next_tetromino();
    } else {
        const auto on_hold = m_tetromino_on_hold->type();
        m_tetromino_on_hold = Tetromino{ Grid::hold_tetromino_position, m_active_tetromino->type() };
        spawn_next_tetromino(on_hold);
    }
}


void Tetrion::reset_lock_delay() {
    m_lock_delay_step_index = Application::simulation_step_index() + lock_delay;
}
void Tetrion::refresh_texts() {
    auto i = 0;
    std::stringstream stream;
    stream << "score: " << m_score;
    m_text_rows.at(i++).set_text(stream.str());

    stream = {};
    stream << "level: " << m_level;
    m_text_rows.at(i++).set_text(stream.str());

    stream = {};
    stream << "lines: " << m_lines_cleared;
    m_text_rows.at(i++).set_text(stream.str());

    if (m_use_player_text and m_player_num.has_value()) {
        stream = {};
        // for humans it' more readable, when it's 1 indexed
        stream << "player " << m_player_num.value() + 1;
        m_text_rows.at(i++).set_text(stream.str());
    }
}

void Tetrion::clear_fully_occupied_lines() {
    bool cleared = false;
    const int lines_cleared_before = m_lines_cleared;
    do {
        cleared = false;
        for (int row = 0; row < Grid::height; ++row) {
            bool fully_occupied = true;
            for (int column = 0; column < Grid::width; ++column) {
                if (m_mino_stack.is_empty(Point{ column, row })) {
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
                }
                m_mino_stack.clear_row_and_let_sink(row);
                cleared = true;
                break;
            }
        }
    } while (cleared);
    const int num_lines_cleared = m_lines_cleared - lines_cleared_before;
    static constexpr std::array<int, 5> score_per_line_multiplier{ 0, 40, 100, 300, 1200 };
    m_score += score_per_line_multiplier.at(num_lines_cleared) * (m_level + 1);
}

void Tetrion::lock_active_tetromino() {
    assert(m_active_tetromino.has_value());
    for (const Mino& mino : m_active_tetromino->minos()) {
        m_mino_stack.set(mino.position(), mino.type());
    }
    m_allowed_to_hold = true;
    m_is_in_lock_delay = false;
    m_num_executed_lock_delays = 0;
    clear_fully_occupied_lines();
    spawn_next_tetromino();
    refresh_texts();
    reset_lock_delay();

    // save a snapshot on every freeze (only in debug builds)
#ifdef DEBUG_BUILD
    if (m_recording_writer) {
        (*m_recording_writer)->add_snapshot(m_tetrion_index, Application::simulation_step_index(), *this);
    }
#endif
}

bool Tetrion::is_active_tetromino_position_valid() const {
    if (not m_active_tetromino) {
        return false;
    }
    return is_tetromino_position_valid(*m_active_tetromino);
}

bool Tetrion::is_valid_mino_position(Point position) const {
    return position.x >= 0 and position.x < Grid::width and position.y >= 0 and position.y < Grid::height
           and m_mino_stack.is_empty(position);
}

bool Tetrion::is_active_tetromino_completely_visible() const {
    if (not m_active_tetromino) {
        return false;
    }
    for (const Mino& mino : m_active_tetromino->minos()) {
        if (mino.position().y < Grid::invisible_rows) {
            return false;
        }
    }
    return true;
}

void Tetrion::refresh_ghost_tetromino() {
    if (not m_active_tetromino.has_value()) {
        m_ghost_tetromino = {};
        return;
    }
    m_ghost_tetromino = *m_active_tetromino;
    while (is_tetromino_position_valid(*m_ghost_tetromino)) {
        m_ghost_tetromino->move_down();
    }
    m_ghost_tetromino->move_up();
}

void Tetrion::refresh_preview() {
    m_preview_tetromino = Tetromino{ Grid::preview_tetromino_position, m_sequence_bags[0][m_sequence_index] };
}

TetrominoType Tetrion::get_next_tetromino_type() {
    const TetrominoType next_type = m_sequence_bags[0][m_sequence_index];
    m_sequence_index = (m_sequence_index + 1) % Bag::size();
    if (m_sequence_index == 0) {
        // we had a wrap-around
        m_sequence_bags[0] = m_sequence_bags[1];
        m_sequence_bags[1] = Bag{ m_random };
    }
    return next_type;
}

bool Tetrion::is_tetromino_position_valid(const Tetromino& tetromino) const {
    for (const Mino& mino : tetromino.minos()) {
        if (not is_valid_mino_position(mino.position())) {
            return false;
        }
    }
    return true;
}

bool Tetrion::rotate(Tetrion::RotationDirection rotation_direction) {
    if (not m_active_tetromino) {
        return false;
    }

    const auto wall_kick_table = get_wall_kick_table();
    if (not wall_kick_table.has_value()) {
        return false;
    }

    const auto from_rotation = m_active_tetromino->rotation();
    const auto to_rotation = from_rotation + (rotation_direction == RotationDirection::Left ? -1 : 1);
    const auto table_index = rotation_to_index(from_rotation, to_rotation);

    if (rotation_direction == RotationDirection::Left) {
        m_active_tetromino->rotate_left();
    } else {
        m_active_tetromino->rotate_right();
    }

    for (const auto translation : wall_kick_table->at(table_index)) {
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

bool Tetrion::move(const Tetrion::MoveDirection move_direction) {
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

    assert(false and "unreachable");
    return false;
}

tl::optional<const Tetrion::WallKickTable&> Tetrion::get_wall_kick_table() const {
    assert(m_active_tetromino.has_value() and "no active tetromino");
    const auto type = m_active_tetromino->type();
    switch (type) {
        case TetrominoType::J:
        case TetrominoType::L:
        case TetrominoType::T:
        case TetrominoType::S:
        case TetrominoType::Z:
            return wall_kick_data_jltsz;
        case TetrominoType::I:
            return wall_kick_data_i;
        case TetrominoType::O:
            return {};
    }
    assert(false and "unreachable");
    return {};
}
