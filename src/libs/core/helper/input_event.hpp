#pragma once

#include "./types.hpp"

enum class InputEvent : u8 {
    RotateLeftPressed = 0,
    RotateRightPressed,
    MoveLeftPressed,
    MoveRightPressed,
    MoveDownPressed,
    DropPressed,
    HoldPressed,
    RotateLeftReleased,
    RotateRightReleased,
    MoveLeftReleased,
    MoveRightReleased,
    MoveDownReleased,
    DropReleased,
    HoldReleased,
};
