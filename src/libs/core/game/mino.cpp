#include "./mino.hpp"

[[nodiscard]] helper::TetrominoType Mino::type() const {
    return m_type;
}

[[nodiscard]] const grid::GridUPoint& Mino::position() const {
    return m_position;
}

[[nodiscard]] grid::GridUPoint& Mino::position() {
    return m_position;
}

[[nodiscard]] bool Mino::operator==(const Mino& other) const {
    return m_position == other.m_position and m_type == other.m_type;
}

[[nodiscard]] bool Mino::operator!=(const Mino& other) const {
    return not(*this == other);
}
