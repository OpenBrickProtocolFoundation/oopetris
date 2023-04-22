#pragma once

#include "point.hpp"
#include "tetromino_type.hpp"
#include "types.hpp"
#include <array>

struct Application;
struct Grid;

enum class MinoTransparency : u8 {
    Preview0,
    Preview1,
    Preview2,
    Preview3,
    Preview4,
    Preview5,
    Ghost = 50,
    Solid = 255,
};

struct Mino final {
private:
    Point m_position;
    TetrominoType m_type;
    static constexpr int inset = 3;

public:
    explicit constexpr Mino(Point coords, TetrominoType type) : m_position{ coords }, m_type{ type } { }

    void render(const Application& app, const Grid& grid, MinoTransparency transparency) const;

    [[nodiscard]] TetrominoType type() const {
        return m_type;
    }

    [[nodiscard]] Point position() const {
        return m_position;
    }

    [[nodiscard]] Point& position() {
        return m_position;
    }

    [[nodiscard]] bool operator==(const Mino& other) const {
        return m_position == other.m_position and m_type == other.m_type;
    }

    [[nodiscard]] bool operator!=(const Mino& other) const {
        return not(*this == other);
    }
};
