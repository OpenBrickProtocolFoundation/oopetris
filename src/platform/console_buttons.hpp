

#pragma once

#if defined(__CONSOLE__)

#if defined(__SWITCH__)


#include <switch.h>

// some switch buttons, from libnx, but since SDL doesn't handle inputs as flags, like libnx, the have to be reversed and reversing 1 << x = log_2(x), this is done constexpr


namespace {
    // this is rounding down since >> 1 throws away the least significant bit, but if its a power of two it is spot on
    constexpr unsigned long BITL_REVERSE(unsigned long x) {
        return x == 1 ? 0 : 1 + BITL_REVERSE(x >> 1);
    }
}; // namespace

//using C enum (not enum class) on purpose
enum JOYCON {

    // see /opt/devkitpro/libnx/include/switch/services/hid.h

    JOYCON_A = BITL_REVERSE(HidNpadButton_A), ///< A button / Right face button
    JOYCON_B = BITL_REVERSE(HidNpadButton_B), ///< B button / Down face button
    JOYCON_X = BITL_REVERSE(HidNpadButton_X), ///< X button / Up face button
    JOYCON_Y = BITL_REVERSE(HidNpadButton_Y), ///< Y button / Left face button

    //NOTE: stick buttons are not key events, but can be moved, these need separate handling
    JOYCON_STICK_L = BITL(HidNpadButton_StickL), ///< Left Stick button
    JOYCON_STICK_R = BITL(HidNpadButton_StickR), ///< Right Stick button

    JOYCON_L = BITL_REVERSE(HidNpadButton_L), ///< L button
    JOYCON_R = BITL(HidNpadButton_R),         ///< R button
    JOYCON_ZL = BITL(HidNpadButton_ZL),       ///< ZL button
    JOYCON_ZR = BITL(HidNpadButton_ZR),       ///< ZR button


    JOYCON_PLUS = BITL_REVERSE(HidNpadButton_Plus),   ///< Plus button
    JOYCON_MINUS = BITL_REVERSE(HidNpadButton_Minus), ///< Minus button

    // DPAD alias CROSS
    JOYCON_DPAD_LEFT = BITL_REVERSE(HidNpadButton_Left),   ///< D-Pad Left button
    JOYCON_DPAD_UP = BITL_REVERSE(HidNpadButton_Up),       ///< D-Pad Up button
    JOYCON_DPAD_RIGHT = BITL_REVERSE(HidNpadButton_Right), ///< D-Pad Right button
    JOYCON_DPAD_DOWN = BITL_REVERSE(HidNpadButton_Down),   ///< D-Pad Down button

    JOYCON_LDPAD_LEFT = BITL_REVERSE(HidNpadButton_StickLLeft),   ///< Left Stick pseudo-button when moved Left
    JOYCON_LDPAD_UP = BITL_REVERSE(HidNpadButton_StickLUp),       ///< Left Stick pseudo-button when moved Up
    JOYCON_LDPAD_RIGHT = BITL_REVERSE(HidNpadButton_StickLRight), ///< Left Stick pseudo-button when moved Right
    JOYCON_LDPAD_DOWN = BITL_REVERSE(HidNpadButton_StickLDown),   ///< Left Stick pseudo-button when moved Down

    JOYCON_RDPAD_LEFT = BITL_REVERSE(HidNpadButton_StickRLeft),   ///< Right Stick pseudo-button when moved Left
    JOYCON_RDPAD_UP = BITL_REVERSE(HidNpadButton_StickRUp),       ///< Right Stick pseudo-button when moved Up
    JOYCON_RDPAD_RIGHT = BITL_REVERSE(HidNpadButton_StickRRight), ///< Right Stick pseudo-button when moved Right
    JOYCON_RDPAD_DOWN = BITL_REVERSE(HidNpadButton_StickRDown),   ///< Right Stick pseudo-button when moved Left


    JOYCON_JOYCONL_SL = BITL_REVERSE(HidNpadButton_LeftSL), ///< SL button on Left Joy-Con
    JOYCON_JOYCONL_SR = BITL_REVERSE(HidNpadButton_LeftSR), ///< SR button on Left Joy-Con

    JOYCON_JOYCONR_SL = BITL_REVERSE(HidNpadButton_RightSL), ///< SL button on Right Joy-Con
    JOYCON_JOYCONR_SL = BITL_REVERSE(HidNpadButton_RightSR), ///< SR button on Right Joy-Con

    JOYCON_VERIFICATION = BITL_REVERSE(HidNpadButton_Verification), ///< Verification

};

// some static asserts to check if BITL_REVERSE works as expected
static_assert(BITL(JOYCON_B) == HidNpadButton_B);
static_assert(BITL(JOYCON_PLUS) == HidNpadButton_Plus);

#elif defined(__3DS__)


#include <3ds.h>

// some 3ds buttons, from libctru, but since SDL doesn't handle inputs as flags, like libctru, the have to be reversed and reversing 1 << x = log_2(x), this is done constexpr


namespace {
    // this is rounding down since >> 1 throws away the least significant bit, but if its a power of two it is spot on
    constexpr unsigned int BIT_REVERSE(unsigned int x) {
        return x == 1 ? 0 : 1 + BIT_REVERSE(x >> 1);
    }
}; // namespace

//using C enum (not enum class) on purpose
enum JOYCON {

    // see /opt/devkitpro/libctru/include/3ds/services/hid.h

    JOYCON_A = BIT_REVERSE(KEY_A), ///< A
    JOYCON_B = BIT_REVERSE(KEY_B), ///< B

    JOYCON_SELECT = BIT_REVERSE(KEY_SELECT), ///< Select
    JOYCON_START = BIT_REVERSE(KEY_START),   ///< Start

    JOYCON_DPAD_RIGHT = BIT_REVERSE(KEY_DRIGHT), ///< D-Pad Right
    JOYCON_DPAD_LEFT = BIT_REVERSE(KEY_DLEFT),   ///< D-Pad Left
    JOYCON_DPAD_UP = BIT_REVERSE(KEY_DUP),       ///< D-Pad Up
    JOYCON_DPAD_DOWN = BIT_REVERSE(KEY_DDOWN),   ///< D-Pad Down

    JOYCON_R = BIT_REVERSE(KEY_R), ///< R
    JOYCON_L = BIT_REVERSE(KEY_L), ///< L
    JOYCON_X = BIT_REVERSE(KEY_X), ///< X
    JOYCON_Y = BIT_REVERSE(KEY_Y), ///< Y

    JOYCON_ZL = BIT_REVERSE(KEY_ZL), ///< ZL (New 3DS only)
    JOYCON_ZR = BIT_REVERSE(KEY_ZR), ///< ZR (New 3DS only)

    // has to be handled separately, but MIGHT be used in some form by SDL2
    JOYCON_TOUCH = BIT_REVERSE(KEY_TOUCH), ///< Touch (Not actually provided by HID)

    JOYCON_CSTICK_RIGHT = BIT_REVERSE(KEY_CSTICK_RIGHT), ///< C-Stick Right (New 3DS only)
    JOYCON_CSTICK_LEFT = BIT_REVERSE(KEY_CSTICK_LEFT),   ///< C-Stick Left (New 3DS only)
    JOYCON_CSTICK_UP = BIT_REVERSE(KEY_CSTICK_UP),       ///< C-Stick Up (New 3DS only)
    JOYCON_CSTICK_DOWN = BIT_REVERSE(KEY_CSTICK_DOWN),   ///< C-Stick Down (New 3DS only)

    JOYCON_CPAD_RIGHT = BIT_REVERSE(KEY_CPAD_RIGHT), ///< Circle Pad Right
    JOYCON_CPAD_LEFT = BIT_REVERSE(KEY_CPAD_LEFT),   ///< Circle Pad Left
    JOYCON_CPAD_UP = BIT_REVERSE(KEY_CPAD_UP),       ///< Circle Pad Up
    JOYCON_CPAD_DOWN = BIT_REVERSE(KEY_CPAD_DOWN),   ///< Circle Pad Down

};

// some static asserts to check if BIT_REVERSE works as expected
static_assert(BIT(JOYCON_B) == KEY_B);
static_assert(BIT(JOYCON_SELECT) == KEY_SELECT);


#endif


#endif
