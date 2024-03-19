
#include "tetromino_type.hpp"


[[nodiscard]] Color helper::get_foreground_color(TetrominoType type, u8 alpha) {
    switch (type) {
        case TetrominoType::I:
            return { 0, 240, 240, alpha };
        case TetrominoType::J:
            return { 0, 0, 240, alpha };
        case TetrominoType::L:
            return { 240, 160, 0, alpha };
        case TetrominoType::O:
            return { 240, 240, 0, alpha };
        case TetrominoType::S:
            return { 0, 240, 0, alpha };
        case TetrominoType::T:
            return { 160, 0, 240, alpha };
        case TetrominoType::Z:
            return { 240, 0, 0, alpha };
        default:
            return Color::white();
    }
}

[[nodiscard]] Color helper::get_background_color(TetrominoType type, u8 alpha) {
    switch (type) {
        case TetrominoType::I:
            return { 1, 118, 118, alpha };
        case TetrominoType::J:
            return { 0, 0, 108, alpha };
        case TetrominoType::L:
            return { 118, 79, 1, alpha };
        case TetrominoType::O:
            return { 107, 107, 0, alpha };
        case TetrominoType::S:
            return { 0, 118, 0, alpha };
        case TetrominoType::T:
            return { 80, 0, 120, alpha };
        case TetrominoType::Z:
            return { 118, 0, 0, alpha };
        default:
            return Color::white();
    }
}
