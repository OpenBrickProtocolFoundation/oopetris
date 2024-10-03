#pragma once

#include <core/game/tetromino_type.hpp>
#include <core/helper/random.hpp>

#include "../helper/windows.hpp"
#include <array>

struct Bag final {
private:
    std::array<helper::TetrominoType, static_cast<int>(helper::TetrominoType::LastType) + 1> m_tetromino_sequence;

public:
    OOPETRIS_GRAPHICS_EXPORTED explicit Bag(Random& random);

    static constexpr int size() {
        return static_cast<int>(helper::TetrominoType::LastType) + 1;
    }

    OOPETRIS_GRAPHICS_EXPORTED const helper::TetrominoType& operator[](int index) const;

private:
    static helper::TetrominoType get_random_tetromino_type(Random& random);
};
