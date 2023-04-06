#include "bag.hpp"
#include <cstdlib>
#include <ctime>
#include <iostream>

bool Bag::s_seeded = false;

Bag::Bag() : m_tetromino_sequence{} {
    if (not s_seeded) {
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        s_seeded = true;
    }

    // initialize array with invalid tetromino type
    for (TetrominoType& type : m_tetromino_sequence) {
        type = static_cast<TetrominoType>(static_cast<int>(TetrominoType::LastType) + 1);
    }

    // fill in the sequence with random order
    for (std::size_t i = 0; i < m_tetromino_sequence.size(); ++i) {
        bool type_is_okay = true;
        do {
            type_is_okay = true;
            m_tetromino_sequence[i] = get_random_tetromino_type();
            for (std::size_t j = 0; j < i; ++j) {
                if (m_tetromino_sequence[j] == m_tetromino_sequence[i]) {
                    type_is_okay = false;
                    break;
                }
            }
            if (type_is_okay) {
                std::cerr << static_cast<int>(m_tetromino_sequence[i]) << " ";
            }
        } while (not type_is_okay);
    }
    std::cerr << "\n";
}

TetrominoType Bag::get_random_tetromino_type() {
    return static_cast<TetrominoType>(std::rand() % (static_cast<int>(TetrominoType::LastType) + 1));
}
