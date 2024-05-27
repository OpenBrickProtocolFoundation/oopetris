#pragma once

#include "../helper/point.hpp"
#include "../helper/types.hpp"
#include "./grid_properties.hpp"
#include "./tetromino_type.hpp"

#include <functional>
struct Mino final {
public:
    using GridPoint = grid::GridPoint;
    using ScreenCordsFunction = std::function<shapes::UPoint(const GridPoint&)>;

private:
    GridPoint m_position;
    helper::TetrominoType m_type;

public:
    explicit constexpr Mino(GridPoint position, helper::TetrominoType type) : m_position{ position }, m_type{ type } { }

    [[nodiscard]] helper::TetrominoType type() const;

    [[nodiscard]] const GridPoint& position() const;

    [[nodiscard]] GridPoint& position();

    [[nodiscard]] bool operator==(const Mino& other) const;

    [[nodiscard]] bool operator!=(const Mino& other) const;
};
