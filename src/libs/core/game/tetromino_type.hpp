#pragma once

#include "../helper/color.hpp"
#include "../helper/types.hpp"
#include "../helper/windows.hpp"

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

    OOPETRIS_CORE_EXPORTED [[nodiscard]] Color get_foreground_color(TetrominoType type, u8 alpha);

    OOPETRIS_CORE_EXPORTED [[nodiscard]] Color get_background_color(TetrominoType type, u8 alpha);

} // namespace helper
