

#pragma once

#if not defined(__SWITCH__)
#error "this header is switch only"
#endif

#include "switch.h"

// some switch buttons, from libnx, but since SDL doesn't handle inputs as flags, like libnx, the have to be reversed and reversing 1 << x = log_2(x), this is done constexpr


namespace {
    // this is rounding down since >> 1 throws away the least significant bit, but if its a power of two it is spot on
    constexpr unsigned BITL_REVERSE(unsigned x) {
        return x == 1 ? 0 : 1 + BITL_REVERSE(x >> 1);
    }
}; // namespace

//using C enum (not enum class) on purpose
enum JOYCON {

    JOYCON_A = BITL_REVERSE(HidNpadButton_A),
    JOYCON_B = BITL_REVERSE(HidNpadButton_B),
    JOYCON_X = BITL_REVERSE(HidNpadButton_X),
    JOYCON_Y = BITL_REVERSE(HidNpadButton_Y),

    JOYCON_PLUS = BITL_REVERSE(HidNpadButton_Plus),
    JOYCON_MINUS = BITL_REVERSE(HidNpadButton_Minus),
    JOYCON_CROSS_LEFT = BITL_REVERSE(HidNpadButton_Left),
    JOYCON_CROSS_UP = BITL_REVERSE(HidNpadButton_Up),
    JOYCON_CROSS_RIGHT = BITL_REVERSE(HidNpadButton_Right),
    JOYCON_CROSS_DOWN = BITL_REVERSE(HidNpadButton_Down),
    JOYCON_LDPAD_LEFT = BITL_REVERSE(HidNpadButton_StickLLeft),
    JOYCON_LDPAD_UP = BITL_REVERSE(HidNpadButton_StickLUp),
    JOYCON_LDPAD_RIGHT = BITL_REVERSE(HidNpadButton_StickLRight),
    JOYCON_LDPAD_DOWN = BITL_REVERSE(HidNpadButton_StickLDown),
    JOYCON_RDPAD_LEFT = BITL_REVERSE(HidNpadButton_StickRLeft),
    JOYCON_RDPAD_UP = BITL_REVERSE(HidNpadButton_StickRUp),
    JOYCON_RDPAD_RIGHT = BITL_REVERSE(HidNpadButton_StickRRight),
    JOYCON_RDPAD_DOWN = BITL_REVERSE(HidNpadButton_StickRDown)

};

// some static asserts to check if BITL_REVERSE works as expected
static_assert(BITL(JOYCON_B) == HidNpadButton_B);
static_assert(BITL(JOYCON_PLUS) == HidNpadButton_Plus);
