#pragma once

#include "../helper/types.hpp"
#include "./mino.hpp"

#include <vector>

struct MinoStack final {
private:
    using GridPoint = Mino::GridPoint;
    using ScreenCordsFunction = Mino::ScreenCordsFunction;

    std::vector<Mino> m_minos;

public:
    void clear_row_and_let_sink(u8 row);
    [[nodiscard]] bool is_empty(GridPoint coordinates) const;
    void set(GridPoint coordinates, helper::TetrominoType type);

    [[nodiscard]] u32 num_minos() const;

    [[nodiscard]] const std::vector<Mino>& minos() const;

    [[nodiscard]] bool operator==(const MinoStack& other) const;

    [[nodiscard]] bool operator!=(const MinoStack& other) const;
};

std::ostream& operator<<(std::ostream& ostream, const MinoStack& mino_stack);
