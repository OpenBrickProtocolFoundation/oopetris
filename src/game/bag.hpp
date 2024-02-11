#pragma once

#include "helper/random.hpp"
#include "game/tetromino_type.hpp"
#include <array>


struct Bag final {
private:
    std::array<TetrominoType, static_cast<int>(TetrominoType::LastType) + 1> m_tetromino_sequence;

public:
    explicit Bag(Random& random);

    static constexpr int size() {
        return static_cast<int>(TetrominoType::LastType) + 1;
    }

    TetrominoType operator[](int index) const {
        return m_tetromino_sequence.at(static_cast<usize>(index));
    }

private:
    static TetrominoType get_random_tetromino_type(Random& random);
};
