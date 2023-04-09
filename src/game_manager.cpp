#include "game_manager.hpp"
#include "application.hpp"
#include "utils.hpp"
#include <cassert>
#include <filesystem>
#include <fstream>
#include <spdlog/spdlog.h>
#include <sstream>
#include <utility>

GameManager::GameManager(const Random::Seed random_seed, const int starting_level, const bool record_game)
    : m_random{ random_seed },
      m_grid{ Point::zero(), tile_size },
      m_level{ starting_level },
      m_next_gravity_simulation_step_index{ get_gravity_delay_frames() },
      m_recording{ random_seed },
      m_record_game{ record_game },
      m_lock_delay_step_index{ Application::simulation_step_index() + lock_delay } {
    m_fonts.push_back(std::make_shared<Font>("assets/fonts/PressStart2P.ttf", 18));
    m_score_text = Text{
        Point{ m_grid.to_screen_coords(Grid::preview_tetromino_position + Point{ 0, Grid::preview_extends.y }) },
        Color::white(), "score: 0", m_fonts.front()
    };
    m_level_text = Text{
        Point{ m_grid.to_screen_coords(Grid::preview_tetromino_position + Point{ 0, Grid::preview_extends.y + 1 }) },
        Color::white(), "level: 0", m_fonts.front()
    };
    m_cleared_lines_text = Text{
        Point{ m_grid.to_screen_coords(Grid::preview_tetromino_position + Point{ 0, Grid::preview_extends.y + 2 }) },
        Color::white(), "lines: 0", m_fonts.front()
    };
}

void GameManager::update() {
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

void GameManager::render(const Application& app) const {
    m_grid.render(app);
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
    m_score_text.render(app);
    m_level_text.render(app);
    m_cleared_lines_text.render(app);
}

bool GameManager::handle_input_event(InputEvent event) {
    m_recording.add_record(Application::simulation_step_index(), event);
    switch (event) {
        case InputEvent::RotateLeft:
            if (rotate_tetromino_left()) {
                reset_lock_delay();
                return true;
            }
            return false;
        case InputEvent::RotateRight:
            if (rotate_tetromino_right()) {
                reset_lock_delay();
                return true;
            }
            return false;
        case InputEvent::MoveLeft:
            if (move_tetromino_left()) {
                reset_lock_delay();
                return true;
            }
            return false;
        case InputEvent::MoveRight:
            if (move_tetromino_right()) {
                reset_lock_delay();
                return true;
            }
            return false;
        case InputEvent::MoveDown:
            m_down_key_pressed = true;
            m_is_accelerated_down_movement = true;
            m_next_gravity_simulation_step_index = Application::simulation_step_index() + get_gravity_delay_frames();
            if (move_tetromino_down(MovementType::Forced)) {
                reset_lock_delay();
                return true;
            }
            return false;
        case InputEvent::Drop:
            m_lock_delay_step_index = Application::simulation_step_index(); // lock instantly
            return drop_tetromino();
        case InputEvent::ReleaseMoveDown: {
            m_down_key_pressed = false;
            return false;
        }
        case InputEvent::Hold:
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

void GameManager::spawn_next_tetromino() {
    spawn_next_tetromino(get_next_tetromino_type());
}

void GameManager::spawn_next_tetromino(const TetrominoType type) {
    static constexpr Point spawn_position{ 3, 0 };
    m_active_tetromino = Tetromino{ spawn_position, type };
    refresh_preview();
    if (not is_active_tetromino_position_valid()) {
        m_game_state = GameState::GameOver;
        spdlog::info("game over");
        if (m_record_game) {
            save_recording();
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
}

bool GameManager::rotate_tetromino_right() {
    return with_lock_delay([&]() { return rotate(RotationDirection::Right); });
}

bool GameManager::rotate_tetromino_left() {
    return with_lock_delay([&]() { return rotate(RotationDirection::Left); });
}

bool GameManager::move_tetromino_down(MovementType movement_type) {
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

bool GameManager::move_tetromino_left() {
    return with_lock_delay([&]() { return move(MoveDirection::Left); });
}

bool GameManager::move_tetromino_right() {
    return with_lock_delay([&]() { return move(MoveDirection::Right); });
}

bool GameManager::drop_tetromino() {
    if (not m_active_tetromino) {
        return false;
    }
    int num_movements = 0;
    while (is_active_tetromino_position_valid()) {
        ++num_movements;
        m_active_tetromino->move_down();
    }
    m_active_tetromino->move_up();
    lock_active_tetromino();
    m_score += 4 * num_movements;
    return num_movements > 0;
}

void GameManager::hold_tetromino() {
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

void GameManager::reset_lock_delay() {
    m_lock_delay_step_index = Application::simulation_step_index() + lock_delay;
}

void GameManager::refresh_texts() {
    std::stringstream stream;
    stream << "score: " << m_score;
    m_score_text.set_text(stream.str());

    stream = {};
    stream << "level: " << m_level;
    m_level_text.set_text(stream.str());

    stream = {};
    stream << "lines: " << m_lines_cleared;
    m_cleared_lines_text.set_text(stream.str());
}

void GameManager::clear_fully_occupied_lines() {
    bool cleared = false;
    const int lines_cleared_before = m_lines_cleared;
    do {
        cleared = false;
        for (int row = 0; row < Grid::height; ++row) {
            bool fully_occupied = true;
            for (int column = 0; column < Grid::width; ++column) {
                if (m_grid.is_empty(Point{ column, row })) {
                    fully_occupied = false;
                    break;
                }
            }

            if (fully_occupied) {
                ++m_lines_cleared;
                if (m_lines_cleared % 10 == 0) {
                    ++m_level;
                    spdlog::info("new level: {}", m_level);
                }
                m_grid.clear_row_and_let_sink(row);
                cleared = true;
                break;
            }
        }
    } while (cleared);
    const int num_lines_cleared = m_lines_cleared - lines_cleared_before;
    static constexpr std::array<int, 5> score_per_line_multiplier{ 0, 40, 100, 300, 1200 };
    m_score += score_per_line_multiplier.at(num_lines_cleared) * (m_level + 1);
}

void GameManager::lock_active_tetromino() {
    // this function assumes that m_active_tetromino is not nullptr
    for (const Mino& mino : m_active_tetromino->minos()) {
        m_grid.set(mino.position(), mino.type());
    }
    m_allowed_to_hold = true;
    m_is_in_lock_delay = false;
    m_num_executed_lock_delays = 0;
    clear_fully_occupied_lines();
    spawn_next_tetromino();
    refresh_texts();
    reset_lock_delay();
}

bool GameManager::is_active_tetromino_position_valid() const {
    if (not m_active_tetromino) {
        return false;
    }
    return is_tetromino_position_valid(*m_active_tetromino);
}

bool GameManager::is_valid_mino_position(Point position) const {
    return position.x >= 0 and position.x < Grid::width and position.y >= 0 and position.y < Grid::height
           and m_grid.is_empty(position);
}

bool GameManager::is_active_tetromino_completely_visible() const {
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

void GameManager::refresh_ghost_tetromino() {
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

void GameManager::refresh_preview() {
    m_preview_tetromino = Tetromino{ Grid::preview_tetromino_position, m_sequence_bags[0][m_sequence_index] };
}

TetrominoType GameManager::get_next_tetromino_type() {
    const TetrominoType next_type = m_sequence_bags[0][m_sequence_index];
    m_sequence_index = (m_sequence_index + 1) % Bag::size();
    if (m_sequence_index == 0) {
        // we had a wrap-around
        m_sequence_bags[0] = m_sequence_bags[1];
        m_sequence_bags[1] = Bag{ m_random };
    }
    return next_type;
}

bool GameManager::is_tetromino_position_valid(const Tetromino& tetromino) const {
    for (const Mino& mino : tetromino.minos()) {
        if (not is_valid_mino_position(mino.position())) {
            return false;
        }
    }
    return true;
}

void GameManager::save_recording() const {
    static constexpr auto recordings_directory = "recordings";
    const auto recording_directory_path = std::filesystem::path{ recordings_directory };
    if (not std::filesystem::exists(recording_directory_path)) {
        std::filesystem::create_directory(recording_directory_path);
    }
    const auto filename = fmt::format("{}.rec", utils::current_date_time_iso8601());
    const auto file_path = recording_directory_path / filename;

    spdlog::info("writing recording to file {}", filename);
    std::ofstream file{ file_path, std::ios::out | std::ios::binary };
    if (not file) {
        spdlog::error("unable to write recording to disk");
        return;
    }

    // store seed
    const auto random_seed = m_random.seed();
    file.write(reinterpret_cast<const char*>(&random_seed), sizeof(random_seed));

    for (const auto& record : m_recording) {
        file.write(reinterpret_cast<const char*>(&record.simulation_step_index), sizeof(record.simulation_step_index));
        const auto event = std::to_underlying(record.event);
        file.write(reinterpret_cast<const char*>(&event), sizeof(event));
    }
}

bool GameManager::rotate(GameManager::RotationDirection rotation_direction) {
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

bool GameManager::move(const GameManager::MoveDirection move_direction) {
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

tl::optional<const GameManager::WallKickTable&> GameManager::get_wall_kick_table() const {
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
