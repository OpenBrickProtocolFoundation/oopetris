
#pragma once

#include <SDL.h>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "static_string.hpp"
#include "types.hpp"
#include "utils.hpp"

#if defined(__SWITCH__)
#include "switch_buttons.hpp"
#endif

/* should we replace this by using defines in here, that would be real compile time but I think it' more ugly
e.g
#if defined(__ANDROID__)
#define DEVICE_SUPPORTS_TOUCH
#undef DEVICE_SUPPORTS_KEYS


and then in usage instead of
if (utils::device_supports_touch())

#if defined(DEVICE_SUPPORTS_TOUCH)
 */


namespace utils {

    struct Capabilities {
        bool supports_touch;
        bool supports_keys;
    };

    // the PAUSE and UNPAUSE might be different (e.g on android, even if androids map is stub,
    // it checks in the usage of these for the CrossPlatformAction!), so don't remove the duplication here!
    enum class CrossPlatformAction : u8 {
        OK,
        PAUSE,
        UNPAUSE,
        EXIT,
        DOWN,
        UP,
        LEFT,
        RIGHT,
        CLOSE,
        OPEN_SETTINGS,
    };

    //TODO: support multiple keys
    static std::unordered_map<u8, std::vector<i64>> key_map =
#if defined(__ANDROID__)
            {
                {           static_cast<u8>(CrossPlatformAction::OK), { 0 }},
                {        static_cast<u8>(CrossPlatformAction::PAUSE), { 0 }},
                {      static_cast<u8>(CrossPlatformAction::UNPAUSE), { 0 }},
                {         static_cast<u8>(CrossPlatformAction::EXIT), { 0 }},
                {         static_cast<u8>(CrossPlatformAction::DOWN), { 0 }},
                {           static_cast<u8>(CrossPlatformAction::UP), { 0 }},
                {         static_cast<u8>(CrossPlatformAction::LEFT), { 0 }},
                {        static_cast<u8>(CrossPlatformAction::RIGHT), { 0 }},
                {        static_cast<u8>(CrossPlatformAction::CLOSE), { 0 }},
                {static_cast<u8>(CrossPlatformAction::OPEN_SETTINGS), { 0 }}
    };
#elif defined(__SWITCH__)
            {
                {           static_cast<u8>(CrossPlatformAction::OK),{ JOYCON_A }                                                                     },
                {        static_cast<u8>(CrossPlatformAction::PAUSE),                                       { JOYCON_PLUS }},
                {      static_cast<u8>(CrossPlatformAction::UNPAUSE),                                       { JOYCON_PLUS }},
                {         static_cast<u8>(CrossPlatformAction::EXIT),                                      { JOYCON_MINUS }},
                {         static_cast<u8>(CrossPlatformAction::DOWN),
                 { JOYCON_CROSS_DOWN, JOYCON_LDPAD_DOWN, JOYCON_RDPAD_DOWN }                                               },
                {           static_cast<u8>(CrossPlatformAction::UP), { JOYCON_CROSS_UP, JOYCON_LDPAD_UP, JOYCON_RDPAD_UP }},
                {         static_cast<u8>(CrossPlatformAction::LEFT),
                 { JOYCON_CROSS_LEFT, JOYCON_LDPAD_LEFT, JOYCON_RDPAD_LEFT }                                               },
                {        static_cast<u8>(CrossPlatformAction::RIGHT),
                 { JOYCON_CROSS_RIGHT, JOYCON_LDPAD_RIGHT, JOYCON_RDPAD_RIGHT }                                            },
                {        static_cast<u8>(CrossPlatformAction::CLOSE),                                      { JOYCON_MINUS }},
                {static_cast<u8>(CrossPlatformAction::OPEN_SETTINGS),                                          { JOYCON_Y }},
};
#else
            {
                {           static_cast<u8>(CrossPlatformAction::OK), { SDLK_RETURN, SDLK_SPACE }},
                {        static_cast<u8>(CrossPlatformAction::PAUSE),             { SDLK_ESCAPE }},
                {      static_cast<u8>(CrossPlatformAction::UNPAUSE),             { SDLK_ESCAPE }},
                {         static_cast<u8>(CrossPlatformAction::EXIT),             { SDLK_RETURN }},
                {         static_cast<u8>(CrossPlatformAction::DOWN),       { SDLK_DOWN, SDLK_s }},
                {           static_cast<u8>(CrossPlatformAction::UP),         { SDLK_UP, SDLK_w }},
                {         static_cast<u8>(CrossPlatformAction::LEFT),       { SDLK_LEFT, SDLK_a }},
                {        static_cast<u8>(CrossPlatformAction::RIGHT),      { SDLK_RIGHT, SDLK_d }},
                {        static_cast<u8>(CrossPlatformAction::CLOSE),             { SDLK_ESCAPE }},
                {static_cast<u8>(CrossPlatformAction::OPEN_SETTINGS),                  { SDLK_e }},
};
#endif


    [[nodiscard]] constexpr Capabilities get_capabilities() {
#if defined(__ANDROID__)
        return Capabilities{ true, false };
#elif defined(__SWITCH__)
        return Capabilities{ true, true };
#else
        return Capabilities{ false, true };
#endif
    }

    [[nodiscard]] constexpr bool device_supports_touch() {
        return get_capabilities().supports_touch;
    }

    [[nodiscard]] constexpr bool device_supports_keys() {
        return get_capabilities().supports_keys;
    }

    [[nodiscard]] bool event_is_action(const SDL_Event& event, CrossPlatformAction action);

    [[nodiscard]] std::vector<i64> get_bound_keys();

    [[nodiscard]] constexpr std::string_view action_description(CrossPlatformAction action) {
#if defined(__ANDROID__)
        UNUSED(action);
        return "NOT POSSIBLE";
#elif defined(__SWITCH__)
        switch (action) {
            case CrossPlatformAction::OK:
                return "A";
            case CrossPlatformAction::PAUSE:
            case CrossPlatformAction::UNPAUSE:
                return "PLUS";
            case CrossPlatformAction::CLOSE:
            case CrossPlatformAction::EXIT:
                return "MINUS";
            case CrossPlatformAction::DOWN:
                return "Down";
            case CrossPlatformAction::UP:
                return "Up";
            case CrossPlatformAction::LEFT:
                return "Left";
            case CrossPlatformAction::RIGHT:
                return "Right";
            case CrossPlatformAction::OPEN_SETTINGS:
                return "Y";
            default:
                utils::unreachable();
        }
#else
        UNUSED(action);
        switch (action) {
            case CrossPlatformAction::OK:
                return "Enter";
            case CrossPlatformAction::PAUSE:
            case CrossPlatformAction::UNPAUSE:
            case CrossPlatformAction::CLOSE:
                return "Esc";
            case CrossPlatformAction::EXIT:
                return "Enter";
            case CrossPlatformAction::DOWN:
                return "Down";
            case CrossPlatformAction::UP:
                return "Up";
            case CrossPlatformAction::LEFT:
                return "Left";
            case CrossPlatformAction::RIGHT:
                return "Right";
            case CrossPlatformAction::OPEN_SETTINGS:
                return "E";
            default:
                utils::unreachable();
        }
#endif
    }


} // namespace utils
