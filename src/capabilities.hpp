
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
    enum class CrossPlatformAction : u8 { OK, PAUSE, UNPAUSE, EXIT, DOWN, UP };

    //TODO: support multiple keys
    static std::unordered_map<u8, std::vector<i64>> key_map =
#if defined(__ANDROID__)
            {
                {     static_cast<u8>(CrossPlatformAction::OK), { 0 }},
                {  static_cast<u8>(CrossPlatformAction::PAUSE), { 0 }},
                {static_cast<u8>(CrossPlatformAction::UNPAUSE), { 0 }},
                {   static_cast<u8>(CrossPlatformAction::EXIT), { 0 }},
                {   static_cast<u8>(CrossPlatformAction::DOWN), { 0 }},
                {     static_cast<u8>(CrossPlatformAction::UP), { 0 }}
    };
#elif defined(__SWITCH__)
            {
                { static_cast<u8>(CrossPlatformAction::OK), { JOYCON_A } },
                { static_cast<u8>(CrossPlatformAction::PAUSE), { JOYCON_PLUS } },
                { static_cast<u8>(CrossPlatformAction::UNPAUSE), { JOYCON_PLUS } },
                { static_cast<u8>(CrossPlatformAction::EXIT), { JOYCON_MINUS } },
                { static_cast<u8>(CrossPlatformAction::DOWN),
                  { JOYCON_CROSS_DOWN, JOYCON_LDPAD_DOWN, JOYCON_RDPAD_DOWN } },
                { static_cast<u8>(CrossPlatformAction::UP), { JOYCON_CROSS_UP, JOYCON_LDPAD_UP, JOYCON_RDPAD_UP } }
            };
#else
            {
                { static_cast<u8>(CrossPlatformAction::OK), { SDLK_RETURN, SDLK_SPACE } },
                { static_cast<u8>(CrossPlatformAction::PAUSE), { SDLK_ESCAPE } },
                { static_cast<u8>(CrossPlatformAction::UNPAUSE), { SDLK_ESCAPE } },
                { static_cast<u8>(CrossPlatformAction::EXIT), { SDLK_RETURN } },
                { static_cast<u8>(CrossPlatformAction::DOWN), { SDLK_DOWN, SDLK_s } },
                { static_cast<u8>(CrossPlatformAction::UP), { SDLK_UP, SDLK_w } }
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

    [[nodiscard]] constexpr std::string_view action_description(CrossPlatformAction action) {
#if defined(__ANDROID__)
        switch (action) {
            case CrossPlatformAction::OK:
                return "NOT POSSIBLE";
            case CrossPlatformAction::PAUSE:
                return "Back Button";
            case CrossPlatformAction::UNPAUSE:
                return "Tap anywhere";
            case CrossPlatformAction::EXIT:
                return "Back Button";
            case CrossPlatformAction::DOWN:
                return "NOT POSSIBLE";
            case CrossPlatformAction::UP:
                return "NOT POSSIBLE";
            default:
                utils::unreachable();
        }
#elif defined(__SWITCH__)
        switch (action) {
            case CrossPlatformAction::OK:
                return "A";
            case CrossPlatformAction::PAUSE:
            case CrossPlatformAction::UNPAUSE:
                return "PLUS";
            case CrossPlatformAction::EXIT:
                return "MINUS";
            case CrossPlatformAction::DOWN:
                return "Down";
            case CrossPlatformAction::UP:
                return "Up";
            default:
                utils::unreachable();
        }
#else
        switch (action) {
            case CrossPlatformAction::OK:
                return "Enter";
            case CrossPlatformAction::PAUSE:
                return "Esc";
            case CrossPlatformAction::UNPAUSE:
                return "Esc";
            case CrossPlatformAction::EXIT:
                return "Enter";
            case CrossPlatformAction::DOWN:
                return "Down";
            case CrossPlatformAction::UP:
                return "Up";
            default:
                utils::unreachable();
        }
#endif
    }


} // namespace utils
