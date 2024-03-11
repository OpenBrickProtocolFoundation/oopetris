#include "bag.hpp"

Bag::Bag(Random& random) : m_tetromino_sequence{} {
    // initialize array with invalid tetromino type
    for (helper::TetrominoType& type : m_tetromino_sequence) {
        type = static_cast<helper::TetrominoType>(static_cast<int>(helper::TetrominoType::LastType) + 1);
    }

    // fill in the sequence with random order
    for (usize i = 0; i < m_tetromino_sequence.size(); ++i) {
        while (true) {
            auto type_is_okay = true;
            m_tetromino_sequence.at(i) = get_random_tetromino_type(random);
            for (usize j = 0; j < i; ++j) {
                if (m_tetromino_sequence.at(j) == m_tetromino_sequence.at(i)) {
                    type_is_okay = false;
                    break;
                }
            }
            if (type_is_okay) {
                break;
            }
        }
    }
}

const helper::TetrominoType& Bag::operator[](int index) const {
    return m_tetromino_sequence.at(static_cast<usize>(index));
}


helper::TetrominoType Bag::get_random_tetromino_type(Random& random) {
    return static_cast<helper::TetrominoType>(random.random(static_cast<int>(helper::TetrominoType::LastType) + 1));
}
