#include "mino_stack.hpp"
#include "grid.hpp"
#include <algorithm>
#include <iostream>

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

void MinoStack::draw_minos(const ServiceProvider& service_provider, const Grid& grid) const {
    for (const Mino& mino : m_minos) {
        if (static_cast<usize>(mino.position().y) >= Grid::invisible_rows) {
            mino.render(service_provider, grid, MinoTransparency::Solid);
        }
    }
}

std::ostream& operator<<(std::ostream& ostream, const MinoStack& mino_stack) {
    ostream << "MinoStack(\n";
    for (usize y = 0; y < Grid::height; ++y) {
        for (usize x = 0; x < Grid::width; ++x) {
            const auto find_iterator =
                    std::find_if(mino_stack.minos().cbegin(), mino_stack.minos().cend(), [&](const auto& mino) {
                        return mino.position() == Point{ static_cast<int>(x), static_cast<int>(y) };
                    });
            const auto found = (find_iterator != mino_stack.minos().cend());
            if (found) {
                ostream << magic_enum::enum_name(find_iterator->type());
            } else {
                ostream << " ";
            }
        }
        ostream << "\n";
    }

    ostream << ")";
    return ostream;
}
