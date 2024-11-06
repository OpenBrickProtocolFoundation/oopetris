#pragma once

#include "../helper/point.hpp"
#include "../helper/types.hpp"
#include "../helper/windows.hpp"
#include "./grid_properties.hpp"
#include "./tetromino_type.hpp"

#include <functional>
struct Mino final {
public:
    using ScreenCordsFunction = std::function<shapes::UPoint(const grid::GridPoint&)>;

private:
    grid::GridPoint m_position;
    helper::TetrominoType m_type;

public:
    OOPETRIS_CORE_EXPORTED explicit constexpr Mino(grid::GridPoint position, helper::TetrominoType type)
        : m_position{ position },
          m_type{ type } { }

    OOPETRIS_CORE_EXPORTED [[nodiscard]] helper::TetrominoType type() const;

    OOPETRIS_CORE_EXPORTED [[nodiscard]] const grid::GridPoint& position() const;

    OOPETRIS_CORE_EXPORTED [[nodiscard]] grid::GridPoint& position();

    OOPETRIS_CORE_EXPORTED [[nodiscard]] bool operator==(const Mino& other) const;

    OOPETRIS_CORE_EXPORTED [[nodiscard]] bool operator!=(const Mino& other) const;
};
