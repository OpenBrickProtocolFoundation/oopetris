#pragma once

#include "../helper/color.hpp"
#include "../helper/export_symbols.hpp"
#include "../helper/types.hpp"

namespace helper {

    enum class TetrominoType : u8 {
        I = 0,
        J,
        L,
        O,
        S,
        T,
        Z,
        LastType = Z,
    };

    [[nodiscard]] OOPETRIS_CORE_EXPORTED Color get_foreground_color(TetrominoType type, u8 alpha);

    [[nodiscard]] OOPETRIS_CORE_EXPORTED Color get_background_color(TetrominoType type, u8 alpha);

} // namespace helper
