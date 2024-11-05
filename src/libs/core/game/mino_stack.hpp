#pragma once

#include "../helper/types.hpp"
#include "../helper/windows.hpp"
#include "./mino.hpp"

#include <vector>

struct MinoStack final {
private:
    using ScreenCordsFunction = Mino::ScreenCordsFunction;

    std::vector<Mino> m_minos;

public:
    OOPETRIS_CORE_EXPORTED void clear_row_and_let_sink(u8 row);

    OOPETRIS_CORE_EXPORTED [[nodiscard]] bool is_empty(grid::GridUPoint coordinates) const;

    OOPETRIS_CORE_EXPORTED void set(grid::GridUPoint coordinates, helper::TetrominoType type);

    OOPETRIS_CORE_EXPORTED [[nodiscard]] u32 num_minos() const;

    OOPETRIS_CORE_EXPORTED [[nodiscard]] const std::vector<Mino>& minos() const;

    OOPETRIS_CORE_EXPORTED [[nodiscard]] bool operator==(const MinoStack& other) const;

    OOPETRIS_CORE_EXPORTED [[nodiscard]] bool operator!=(const MinoStack& other) const;
};

OOPETRIS_CORE_EXPORTED std::ostream& operator<<(std::ostream& ostream, const MinoStack& mino_stack);
