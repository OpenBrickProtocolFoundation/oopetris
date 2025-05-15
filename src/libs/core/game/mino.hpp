#pragma once

#include "../helper/export_symbols.hpp"
#include "../helper/point.hpp"
#include "../helper/types.hpp"
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
    explicit constexpr Mino(grid::GridPoint position, helper::TetrominoType type)
        : m_position{ position },
          m_type{ type } { }

    [[nodiscard]] OOPETRIS_CORE_EXPORTED helper::TetrominoType type() const;

    [[nodiscard]] OOPETRIS_CORE_EXPORTED const grid::GridPoint& position() const;

    [[nodiscard]] OOPETRIS_CORE_EXPORTED grid::GridPoint& position();

    [[nodiscard]] OOPETRIS_CORE_EXPORTED bool operator==(const Mino& other) const;

    [[nodiscard]] OOPETRIS_CORE_EXPORTED bool operator!=(const Mino& other) const;
};
