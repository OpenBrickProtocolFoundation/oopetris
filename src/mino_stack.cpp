#include "mino_stack.hpp"
#include "grid.hpp"

void MinoStack::clear_row_and_let_sink(int row) {
    m_minos.erase(
            std::remove_if(m_minos.begin(), m_minos.end(), [&](const Mino& mino) { return mino.position().y == row; }),
            m_minos.end()
    );
    for (Mino& mino : m_minos) {
        if (mino.position().y < row) {
            ++mino.position().y;
        }
    }
}

[[nodiscard]] bool MinoStack::is_empty(Point coordinates) const {
    for (const Mino& mino : m_minos) {
        if (mino.position() == coordinates) {
            return false;
        }
    }
    return true;
}

void MinoStack::set(Point coordinates, TetrominoType type) {
    const Mino to_insert = Mino{ coordinates, type };
    for (Mino& current : m_minos) {
        if (current.position() == coordinates) {
            current = to_insert;
            return;
        }
    }
    m_minos.push_back(to_insert);
}

void MinoStack::draw_minos(const Application& app, const Grid& grid) const {
    for (const Mino& mino : m_minos) {
        if (mino.position().y >= Grid::invisible_rows) {
            mino.render(app, grid, false);
        }
    }
}

std::ostream& operator<<(std::ostream& os, const MinoStack& mino_stack) {
    os << "MinoStack(";
    for (usize i = 0; i < mino_stack.num_minos(); ++i) {
        const auto& mino = mino_stack.minos().at(i);
        os << "{" << mino.position().x << ", " << mino.position().y << ", " << magic_enum::enum_name(mino.type())
           << "}";
        if (i < mino_stack.num_minos() - 1) {
            os << ", ";
        }
    }
    os << ")";
    return os;
}
