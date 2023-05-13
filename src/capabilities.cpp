
#include "capabilities.hpp"
#include "utils.hpp"
#include <SDL.h>

#if defined(__SWITCH__)
#include "switch_buttons.hpp"
#endif

[[nodiscard]] bool utils::event_is_action(const SDL_Event& event, const CrossPlatformAction action) {
#if defined(__ANDROID__)
    switch (action) {
        case CrossPlatformAction::OK:
            // this can't be checked here, it has to be checked via collision on buttons etc. event_is_action(..., ...::OK) can only be used inside device_supports_keys() clauses!
            utils::unreachable();

        case CrossPlatformAction::PAUSE:
            return (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_AC_BACK);

        case CrossPlatformAction::UNPAUSE:
            return event.type == SDL_FINGERDOWN;

        case CrossPlatformAction::EXIT:
            return (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_AC_BACK);

        default:
            utils::unreachable();
    }
#else

    const i64 needed_event = utils::key_map.at(static_cast<u8>(action));
#if defined(__SWITCH__)
    return (event.type == SDL_JOYBUTTONDOWN and event.jbutton.button == needed_event);
#else
    return (event.type == SDL_KEYDOWN and event.key.keysym.sym == needed_event);
#endif

#endif
}
