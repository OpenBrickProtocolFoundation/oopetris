#include "game_manager.hpp"
#include "application.hpp"
#include <iostream>

GameManager::GameManager()
    : m_grid{ Point::zero(), tile_size },
      m_next_gravity_step_time{ Application::elapsed_time() + get_gravity_delay(0) } {
    std::srand(std::time(nullptr));
}

void GameManager::update() {
    switch (m_game_state) {
        case GameState::Playing: {
            const double current_time = Application::elapsed_time();
            if (current_time >= m_next_gravity_step_time) {
                move_tetromino_down();
                m_next_gravity_step_time += get_gravity_delay(m_level);
            }
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
}

void GameManager::spawn_tetromino() {
    static constexpr Point spawn_position{ 3, 0 };
    m_active_tetromino = std::make_unique<Tetromino>(spawn_position, get_random_tetromino_type());
    if (!is_active_tetromino_position_valid()) {
        m_game_state = GameState::GameOver;
        std::cerr << "game over\n";
        m_active_tetromino = {};
        return;
    }
    m_next_gravity_step_time = Application::elapsed_time() + get_gravity_delay(m_level);
}

void GameManager::rotate_tetromino_right() {
    if (!m_active_tetromino) {
        return;
    }
    m_active_tetromino->rotate_right();
    if (!is_active_tetromino_position_valid()) {
        m_active_tetromino->rotate_left();
    }
}

void GameManager::rotate_tetromino_left() {
    if (!m_active_tetromino) {
        return;
    }
    m_active_tetromino->rotate_left();
    if (!is_active_tetromino_position_valid()) {
        m_active_tetromino->rotate_right();
    }
}

void GameManager::move_tetromino_down() {
    if (!m_active_tetromino) {
        return;
    }
    m_active_tetromino->move_down();
    if (!is_active_tetromino_position_valid()) {
        m_active_tetromino->move_up();
        freeze_active_tetromino();
    }
}

void GameManager::move_tetromino_left() {
    if (!m_active_tetromino) {
        return;
    }
    m_active_tetromino->move_left();
    if (!is_active_tetromino_position_valid()) {
        m_active_tetromino->move_right();
    }
}

void GameManager::move_tetromino_right() {
    if (!m_active_tetromino) {
        return;
    }
    m_active_tetromino->move_right();
    if (!is_active_tetromino_position_valid()) {
        m_active_tetromino->move_left();
    }
}

void GameManager::drop_tetromino() {
    if (!m_active_tetromino) {
        return;
    }
    while (is_active_tetromino_position_valid()) {
        m_active_tetromino->move_down();
    }
    m_active_tetromino->move_up();
    freeze_active_tetromino();
}

void GameManager::clear_fully_occupied_lines() {
    bool cleared = false;
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
}

void GameManager::freeze_active_tetromino() {
    // this function assumes that m_active_tetromino is not nullptr
    for (const Mino& mino : m_active_tetromino->minos()) {
        m_grid.set(mino.position(), mino.type());
    }
    clear_fully_occupied_lines();
    spawn_tetromino();
}

bool GameManager::is_active_tetromino_position_valid() const {
    if (!m_active_tetromino) {
        return true;
    }
    for (const Mino& mino : m_active_tetromino->minos()) {
        if (!is_valid_mino_position(mino.position())) {
            return false;
        }
    }
    return true;
}

bool GameManager::is_valid_mino_position(Point position) const {
    return position.x >= 0 && position.x < Grid::width && position.y >= 0 && position.y < Grid::height
           && m_grid.is_empty(position);
}

TetrominoType GameManager::get_random_tetromino_type() {
    return static_cast<TetrominoType>(std::rand() % (static_cast<int>(TetrominoType::LastType) + 1));
}

std::array<int, 30> GameManager::frames_per_tile{ 48, 43, 38, 33, 28, 23, 18, 13, 8, 6, 5, 5, 5, 4, 4,
                                                  4,  3,  3,  3,  2,  2,  2,  2,  2, 2, 2, 2, 2, 2, 1 };
