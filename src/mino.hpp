#pragma once

#include "point.hpp"
#include "tetromino_type.hpp"

struct Application;
struct Grid;

struct Mino final {
private:
    Point m_position;
    TetrominoType m_type;
    static constexpr int inset = 3;

public:
    explicit constexpr Mino(Point coords, TetrominoType type) : m_position{ coords }, m_type{ type } { }

    void render(const Application& app, const Grid& grid, bool as_ghost) const;

    TetrominoType type() const {
        return m_type;
    }

    Point position() const {
        return m_position;
    }

    Point& position() {
        return m_position;
    }

    [[nodiscard]] bool operator==(const Mino& other) const {
        return m_position == other.m_position and m_type == other.m_type;
    }

    [[nodiscard]] bool operator!=(const Mino& other) const {
        return not (*this == other);
    }
};
