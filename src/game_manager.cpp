#include "game_manager.hpp"
#include "application.hpp"
#include <cassert>
#include <iostream>
#include <sstream>

GameManager::GameManager()
    : m_grid{ Point::zero(), tile_size },
      m_next_gravity_step_time{ Application::elapsed_time() + get_gravity_delay() } {
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
            const double current_time = Application::elapsed_time();
            if (current_time >= m_next_gravity_step_time) {
                if (m_is_accelerated_down_movement and not m_down_key_pressed) {
                    m_next_gravity_step_time -= get_gravity_delay();
                    m_is_accelerated_down_movement = false;
                } else {
                    move_tetromino_down(m_is_accelerated_down_movement ? MovementType::Forced : MovementType::Gravity);
                }
                m_next_gravity_step_time += get_gravity_delay();
            }
            refresh_texts();
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
    m_score_text.render(app);
    m_level_text.render(app);
    m_cleared_lines_text.render(app);
}

bool GameManager::handle_input_event(Input::Event event) {
    switch (event) {
        case Input::Event::RotateLeft:
            return rotate_tetromino_left();
        case Input::Event::RotateRight:
            return rotate_tetromino_right();
        case Input::Event::MoveLeft:
            return move_tetromino_left();
        case Input::Event::MoveRight:
            return move_tetromino_right();
        case Input::Event::MoveDown:
            m_down_key_pressed = true;
            m_is_accelerated_down_movement = true;
            m_next_gravity_step_time = Application::elapsed_time() + get_gravity_delay();
            return move_tetromino_down(MovementType::Forced);
        case Input::Event::Drop:
            return drop_tetromino();
        case Input::Event::ReleaseMoveDown: {
            m_down_key_pressed = false;
            return false;
        }
        default:
            assert(false and "unknown event");
            return false;
    }
}

void GameManager::spawn_next_tetromino() {
    static constexpr Point spawn_position{ 3, 0 };
    const TetrominoType next_type = get_next_tetromino_type();
    m_active_tetromino = Tetromino{ spawn_position, next_type };
    refresh_preview();
    if (not is_active_tetromino_position_valid()) {
        m_game_state = GameState::GameOver;
        std::cerr << "game over\n";
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
    m_next_gravity_step_time = Application::elapsed_time() + get_gravity_delay();
}

bool GameManager::rotate_tetromino_right() {
    if (not m_active_tetromino) {
        return false;
    }
    m_active_tetromino->rotate_right();
    if (not is_active_tetromino_position_valid()) {
        m_active_tetromino->rotate_left();
        return false;
    }
    return true;
}

bool GameManager::rotate_tetromino_left() {
    if (not m_active_tetromino) {
        return false;
    }
    m_active_tetromino->rotate_left();
    if (not is_active_tetromino_position_valid()) {
        m_active_tetromino->rotate_right();
        return false;
    }
    return true;
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
        m_active_tetromino->move_up();
        lock_active_tetromino();
        return false;
    }
    return true;
}

bool GameManager::move_tetromino_left() {
    if (not m_active_tetromino) {
        return false;
    }
    m_active_tetromino->move_left();
    if (not is_active_tetromino_position_valid()) {
        m_active_tetromino->move_right();
        return false;
    }
    return true;
}

bool GameManager::move_tetromino_right() {
    if (not m_active_tetromino) {
        return false;
    }
    m_active_tetromino->move_right();
    if (not is_active_tetromino_position_valid()) {
        m_active_tetromino->move_left();
        return false;
    }
    return true;
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
                    std::cerr << "new level: " << m_level << "\n";
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
    clear_fully_occupied_lines();
    spawn_next_tetromino();
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
        m_sequence_bags[1] = Bag{};
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
