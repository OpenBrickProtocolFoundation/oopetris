#pragma once

#include "graphics/point.hpp"
#include "helper/types.hpp"
#include "manager/service_provider.hpp"
#include "tetromino_type.hpp"
#include <array>

struct Grid;

enum class MinoTransparency : u8 {
    // here the enum value is used as index into the preview alpha array
    Preview0,
    Preview1,
    Preview2,
    Preview3,
    Preview4,
    Preview5,
    // here the enum value is used as alpha!
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

    void render(
            const ServiceProvider& service_provider,
            const Grid* grid,
            MinoTransparency transparency,
            const Point& offset = Point::zero()
    ) const;

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
