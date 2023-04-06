#include "tetromino_type.hpp"

Color get_foreground_color(TetrominoType type, std::uint8_t alpha) {
    switch (type) {
        case TetrominoType::I:
            return Color{ 0, 240, 240, alpha };
        case TetrominoType::J:
            return Color{ 0, 0, 240, alpha };
        case TetrominoType::L:
            return Color{ 240, 160, 0, alpha };
        case TetrominoType::O:
            return Color{ 240, 240, 0, alpha };
        case TetrominoType::S:
            return Color{ 0, 240, 0, alpha };
        case TetrominoType::T:
            return Color{ 160, 0, 240, alpha };
        case TetrominoType::Z:
            return Color{ 240, 0, 0, alpha };
        default:
            return Color::white();
    }
}

Color get_background_color(TetrominoType type, std::uint8_t alpha) {
    switch (type) {
        case TetrominoType::I:
            return Color{ 1, 118, 118, alpha };
        case TetrominoType::J:
            return Color{ 0, 0, 108, alpha };
        case TetrominoType::L:
            return Color{ 118, 79, 1, alpha };
        case TetrominoType::O:
            return Color{ 107, 107, 0, alpha };
        case TetrominoType::S:
            return Color{ 0, 118, 0, alpha };
        case TetrominoType::T:
            return Color{ 80, 0, 120, alpha };
        case TetrominoType::Z:
            return Color{ 118, 0, 0, alpha };
        default:
            return Color::white();
    }
}
