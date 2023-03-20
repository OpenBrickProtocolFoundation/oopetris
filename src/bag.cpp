#include "bag.hpp"
#include <cstdlib>
#include <ctime>

bool Bag::s_seeded = false;

Bag::Bag() : m_tetromino_sequence{} {
    if (!s_seeded) {
        std::srand(std::time(nullptr));
    }

    // initialize array with invalid tetromino type
    for (TetrominoType& type : m_tetromino_sequence) {
        type = static_cast<TetrominoType>(static_cast<int>(TetrominoType::LastType) + 1);
    }

    // fill in the sequence with random order
    for (int i = 0; i < m_tetromino_sequence.size(); ++i) {
        bool type_is_okay = true;
        do {
            type_is_okay = true;
            m_tetromino_sequence[i] = get_random_tetromino_type();
            for (int j = 0; j < i; ++j) {
                if (m_tetromino_sequence[j] == m_tetromino_sequence[i]) {
                    type_is_okay = false;
                    break;
                }
            }
        } while (!type_is_okay);
    }
}

TetrominoType Bag::get_random_tetromino_type() {
    return static_cast<TetrominoType>(std::rand() % (static_cast<int>(TetrominoType::LastType) + 1));
}
