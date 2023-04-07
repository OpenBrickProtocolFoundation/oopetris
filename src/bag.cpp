#include "bag.hpp"
#include <cstdlib>
#include <ctime>

Bag::Bag(Random& random) : m_tetromino_sequence{} {
    // initialize array with invalid tetromino type
    for (TetrominoType& type : m_tetromino_sequence) {
        type = static_cast<TetrominoType>(static_cast<int>(TetrominoType::LastType) + 1);
    }

    // fill in the sequence with random order
    for (std::size_t i = 0; i < m_tetromino_sequence.size(); ++i) {
        bool type_is_okay = true;
        do {
            type_is_okay = true;
            m_tetromino_sequence[i] = get_random_tetromino_type(random);
            for (std::size_t j = 0; j < i; ++j) {
                if (m_tetromino_sequence[j] == m_tetromino_sequence[i]) {
                    type_is_okay = false;
                    break;
                }
            }
        } while (not type_is_okay);
    }
}

TetrominoType Bag::get_random_tetromino_type(Random& random) {
    return static_cast<TetrominoType>(random.random(static_cast<int>(TetrominoType::LastType) + 1));
}
