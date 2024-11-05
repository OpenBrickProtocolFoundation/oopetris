#include "./mino_stack.hpp"
#include "./grid_properties.hpp"
#include "./helper/magic_enum_wrapper.hpp"

#include <algorithm>

void MinoStack::clear_row_and_let_sink(u8 row) {
    m_minos.erase(
            std::ranges::remove_if(m_minos, [&](const Mino& mino) { return mino.position().y == row; }).begin(),
            m_minos.end()
    );
    for (Mino& mino : m_minos) {
        if (mino.position().y < row) {
            ++mino.position().y;
        }
    }
}

[[nodiscard]] bool MinoStack::is_empty(grid::GridUPoint coordinates) const {
    return not std::ranges::any_of(m_minos, [&coordinates](const Mino& mino) {
        return mino.position() == coordinates;
    });
}

void MinoStack::set(grid::GridUPoint coordinates, helper::TetrominoType type) {
    const Mino to_insert = Mino{ coordinates, type };
    for (Mino& current : m_minos) {
        if (current.position() == coordinates) {
            current = to_insert;
            return;
        }
    }
    m_minos.push_back(to_insert);
}

[[nodiscard]] u32 MinoStack::num_minos() const {
    return static_cast<u32>(m_minos.size());
}
[[nodiscard]] const std::vector<Mino>& MinoStack::minos() const {
    return m_minos;
}

[[nodiscard]] bool MinoStack::operator==(const MinoStack& other) const {
    if (m_minos.size() != other.m_minos.size()) {
        return false;
    }

    const auto all_of_this_in_other = std::ranges::all_of(m_minos, [&](const auto& mino) {
        return std::ranges::find(other.m_minos, mino) != end(other.m_minos);
    });

    if (not all_of_this_in_other) {
        return false;
    }

    const auto all_of_other_in_this = std::ranges::all_of(other.m_minos, [this](const auto& mino) {
        return std::ranges::find(m_minos, mino) != end(m_minos);
    });

    return all_of_other_in_this;
}

[[nodiscard]] bool MinoStack::operator!=(const MinoStack& other) const {
    return not(*this == other);
}


std::ostream& operator<<(std::ostream& ostream, const MinoStack& mino_stack) {
    ostream << "MinoStack(\n";
    for (u8 y = 0; y < grid::height_in_tiles; ++y) {
        for (u8 x = 0; x < grid::width_in_tiles; ++x) {
            const auto find_iterator = std::ranges::find_if(mino_stack.minos(), [&](const auto& mino) {
                return mino.position() == shapes::AbstractPoint<u8>{ x, y };
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
