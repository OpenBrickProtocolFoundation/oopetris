#pragma once

#include <core/game/tetromino_type.hpp>
#include <core/helper/random.hpp>

#include <array>

struct Bag final {
private:
    std::array<helper::TetrominoType, static_cast<int>(helper::TetrominoType::LastType) + 1> m_tetromino_sequence;

public:
    explicit Bag(Random& random);

    static constexpr int size() {
        return static_cast<int>(helper::TetrominoType::LastType) + 1;
    }

    const helper::TetrominoType& operator[](int index) const;

private:
    static helper::TetrominoType get_random_tetromino_type(Random& random);
};
