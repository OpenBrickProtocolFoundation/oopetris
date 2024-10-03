#pragma once

#include "../helper/point.hpp"
#include "../helper/types.hpp"
#include "../helper/windows.hpp"
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
    OOPETRIS_CORE_EXPORTED explicit constexpr Mino(GridPoint position, helper::TetrominoType type)
        : m_position{ position },
          m_type{ type } { }

    OOPETRIS_CORE_EXPORTED [[nodiscard]] helper::TetrominoType type() const;

    OOPETRIS_CORE_EXPORTED [[nodiscard]] const GridPoint& position() const;

    OOPETRIS_CORE_EXPORTED [[nodiscard]] GridPoint& position();

    OOPETRIS_CORE_EXPORTED [[nodiscard]] bool operator==(const Mino& other) const;

    OOPETRIS_CORE_EXPORTED [[nodiscard]] bool operator!=(const Mino& other) const;
};
