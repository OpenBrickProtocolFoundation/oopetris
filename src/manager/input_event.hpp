#pragma once

#include "helper/types.hpp"

enum class InputEvent : u8 {
    RotateLeftPressed,
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
