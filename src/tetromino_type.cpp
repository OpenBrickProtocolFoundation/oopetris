#include "tetromino_type.hpp"

Color get_foreground_color(TetrominoType type) {
    switch (type) {
        case TetrominoType::I:
            return Color{ 0, 240, 240 };
        case TetrominoType::J:
            return Color{ 0, 0, 240 };
        case TetrominoType::L:
            return Color{ 240, 160, 0 };
        case TetrominoType::O:
            return Color{ 240, 240, 0 };
        case TetrominoType::S:
            return Color{ 0, 240, 0 };
        case TetrominoType::T:
            return Color{ 160, 0, 240 };
        case TetrominoType::Z:
            return Color{ 240, 0, 0 };
        default:
            return Color::white();
    }
}

Color get_background_color(TetrominoType type) {
    switch (type) {
        case TetrominoType::I:
            return Color{ 1, 118, 118 };
        case TetrominoType::J:
            return Color{ 0, 0, 108 };
        case TetrominoType::L:
            return Color{ 118, 79, 1 };
        case TetrominoType::O:
            return Color{ 107, 107, 0 };
        case TetrominoType::S:
            return Color{ 0, 118, 0 };
        case TetrominoType::T:
            return Color{ 80, 0, 120 };
        case TetrominoType::Z:
            return Color{ 118, 0, 0 };
        default:
            return Color::white();
    }
}
