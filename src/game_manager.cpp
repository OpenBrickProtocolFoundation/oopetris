#include "game_manager.hpp"

void GameManager::render(const Application& app) const {
    m_grid.render(app);
    if (m_active_tetromino) {
        m_active_tetromino->render(app, m_grid);
    }
}
