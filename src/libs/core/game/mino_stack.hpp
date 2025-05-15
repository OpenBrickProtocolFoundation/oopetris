#pragma once

#include "../helper/export_symbols.hpp"
#include "../helper/types.hpp"
#include "./mino.hpp"

#include <vector>

struct MinoStack final {
private:
    using ScreenCordsFunction = Mino::ScreenCordsFunction;

    std::vector<Mino> m_minos;

public:
    OOPETRIS_CORE_EXPORTED void clear_row_and_let_sink(u8 row);

    [[nodiscard]] OOPETRIS_CORE_EXPORTED bool is_empty(grid::GridPoint coordinates) const;

    OOPETRIS_CORE_EXPORTED void set(grid::GridPoint coordinates, helper::TetrominoType type);

    [[nodiscard]] OOPETRIS_CORE_EXPORTED u32 num_minos() const;

    [[nodiscard]] OOPETRIS_CORE_EXPORTED const std::vector<Mino>& minos() const;

    [[nodiscard]] OOPETRIS_CORE_EXPORTED bool operator==(const MinoStack& other) const;

    [[nodiscard]] OOPETRIS_CORE_EXPORTED bool operator!=(const MinoStack& other) const;
};

OOPETRIS_CORE_EXPORTED std::ostream& operator<<(std::ostream& ostream, const MinoStack& mino_stack);
