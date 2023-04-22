#include "mino_stack.hpp"
#include "grid.hpp"
#include <algorithm>

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
            mino.render(app, grid, MinoTransparency::Solid);
        }
    }
}

std::ostream& operator<<(std::ostream& os, const MinoStack& mino_stack) {
    os << "MinoStack(\n";
    for (int y = 0; y < Grid::height; ++y) {
        for (int x = 0; x < Grid::width; ++x) {
            const auto find_iterator =
                    std::find_if(mino_stack.minos().cbegin(), mino_stack.minos().cend(), [&](const auto& mino) {
                        return mino.position() == Point{ x, y };
                    });
            const auto found = (find_iterator != mino_stack.minos().cend());
            if (found) {
                os << magic_enum::enum_name(find_iterator->type());
            } else {
                os << " ";
            }
        }
        os << "\n";
    }

    os << ")";
    return os;
}
