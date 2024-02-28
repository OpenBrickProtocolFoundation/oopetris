#pragma once

#include "graphics/point.hpp"
#include "helper/types.hpp"
#include "manager/service_provider.hpp"
#include "tetromino_type.hpp"

#include <array>
#include <functional>

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
    using GridPoint = shapes::AbstractPoint<u8>;
    using ScreenCordsFunction = std::function<shapes::UPoint(const GridPoint&)>;
    GridPoint m_position;
    TetrominoType m_type;
    static constexpr int original_inset = 3;

public:
    explicit constexpr Mino(GridPoint position, TetrominoType type) : m_position{ position }, m_type{ type } { }

    void render(
            const ServiceProvider& service_provider,
            MinoTransparency transparency,
            const double original_scale,
            const ScreenCordsFunction& to_screen_coords,
            const shapes::UPoint& tile_size,
            const GridPoint& offset = GridPoint::zero()
    ) const;

    [[nodiscard]] TetrominoType type() const {
        return m_type;
    }

    [[nodiscard]] GridPoint position() const {
        return m_position;
    }

    [[nodiscard]] GridPoint& position() {
        return m_position;
    }

    [[nodiscard]] bool operator==(const Mino& other) const {
        return m_position == other.m_position and m_type == other.m_type;
    }

    [[nodiscard]] bool operator!=(const Mino& other) const {
        return not(*this == other);
    }
};
