
#include <SDL.h>
#include <vector>

#include "capabilities.hpp"
#include "utils.hpp"


#if defined(__SWITCH__)
#include "switch_buttons.hpp"
#endif

[[nodiscard]] bool utils::event_is_action(const SDL_Event& event, const CrossPlatformAction action) {
#if defined(__ANDROID__)
    switch (action) {
        case CrossPlatformAction::OK:
            // this can't be checked here, it has to be checked via collision on buttons etc. event_is_action(..., ...::OK) can only be used inside device_supports_keys() clauses!
            throw std::runtime_error("Not supported on android");

        case CrossPlatformAction::PAUSE:
            return (event.type == SDL_KEYDOWN and event.key.keysym.sym == SDLK_AC_BACK);

        case CrossPlatformAction::UNPAUSE:
            return event.type == SDL_FINGERDOWN;

        case CrossPlatformAction::EXIT:
        case CrossPlatformAction::CLOSE:
            return (event.type == SDL_KEYDOWN and event.key.keysym.sym == SDLK_AC_BACK);
        case CrossPlatformAction::DOWN:
        case CrossPlatformAction::UP:
        case CrossPlatformAction::LEFT:
        case CrossPlatformAction::RIGHT:
        case CrossPlatformAction::OPEN_SETTINGS:
        case CrossPlatformAction::TAB:
            // this can't be checked here, it has to be checked via collision on buttons etc. event_is_action(..., ...::DOWN, UP ...) can only be used inside device_supports_keys() clauses!
            throw std::runtime_error("Not supported on android");

        default:
            utils::unreachable();
    }
#else

    const std::vector<i64> needed_events = utils::key_map.at(static_cast<u8>(action));
    for (const auto& needed_event : needed_events) {
#if defined(__SWITCH__)
        if (event.type == SDL_JOYBUTTONDOWN and event.jbutton.button == needed_event) {
            return true;
        }
#else
        if (event.type == SDL_KEYDOWN and event.key.keysym.sym == needed_event) {
            return true;
        }
#endif
    }
    return false;
#endif
}


[[nodiscard]] std::vector<i64> utils::get_bound_keys() {
    std::vector<i64> bound_keys{};
    for (const auto& [_, keys] : utils::key_map) {
        for (const auto key : keys) {
            bound_keys.push_back(key);
        }
    }
    return bound_keys;
}


[[nodiscard]] std::vector<i64> utils::get_bound_keys(const std::vector<utils::CrossPlatformAction>& actions) {
    std::vector<i64> bound_keys{};
    for (const auto& [value, keys] : utils::key_map) {
        if (std::find(actions.cbegin(), actions.cend(), static_cast<utils::CrossPlatformAction>(value))
            == actions.cend()) {
            continue;
        }
        for (const auto key : keys) {
            bound_keys.push_back(key);
        }
    }
    return bound_keys;
}


//TODO: not correctly supported ButtonUp, since it only can be triggered, when a ButtonDown event is fired first and the target is not left (unhovered)

[[nodiscard]] bool utils::event_is_click_event(const SDL_Event& event, CrossPlatformClickEvent click_type) {
    SDL_EventType desired_type;
#if defined(__ANDROID__)
    switch (click_type) {
        case CrossPlatformClickEvent::Motion:
            desired_type = SDL_FINGERMOTION;
            break;
        case CrossPlatformClickEvent::ButtonDown:
            desired_type = SDL_FINGERDOWN;
            break;
        case CrossPlatformClickEvent::ButtonUp:
            desired_type = SDL_FINGERUP;
            break;
        case CrossPlatformClickEvent::Any:
            return event.type == SDL_FINGERMOTION || event.type == SDL_FINGERDOWN || event.type == SDL_FINGERUP;
        default:
            utils::unreachable();
    }

#elif defined(__SWITCH__)
    UNUSED(event);
    UNUSED(click_type);
    throw std::runtime_error("Not supported on the Nintendo switch");
#else
    switch (click_type) {
        case CrossPlatformClickEvent::Motion:
            desired_type = SDL_MOUSEMOTION;
            break;
        case CrossPlatformClickEvent::ButtonDown:
            desired_type = SDL_MOUSEBUTTONDOWN;
            break;
        case CrossPlatformClickEvent::ButtonUp:
            desired_type = SDL_MOUSEBUTTONUP;
            break;
        case CrossPlatformClickEvent::Any:
            return event.type == SDL_MOUSEMOTION || event.type == SDL_MOUSEBUTTONDOWN
                   || event.type == SDL_MOUSEBUTTONUP;
        default:
            utils::unreachable();
    }
#endif


    return event.type == desired_type;
}

[[nodiscard]] std::pair<int, int> utils::get_raw_coordinates(const Window* window, const SDL_Event& event) {

#if defined(__ANDROID__)
    // These are doubles, from 0-1 in percent, the have to be casted to absolut x coordinates!
    const double x_percent = event.tfinger.x;
    const double y_percent = event.tfinger.y;
    const auto window_size = window->size();
    const auto x = static_cast<Sint32>(std::round(x_percent * window_size.x));
    const auto y = static_cast<Sint32>(std::round(y_percent * window_size.y));


#elif defined(__SWITCH__)
    UNUSED(window);
    UNUSED(event);
    UNUSED(rect);
    throw std::runtime_error("Not supported on the Nintendo switch");
#else
    UNUSED(window);

    Sint32 x;
    Sint32 y;
    switch (event.type) {
        case SDL_MOUSEMOTION:
            x = event.motion.x;
            y = event.motion.y;
            break;
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
            x = event.button.x;
            y = event.button.y;
            break;
        default:
            utils::unreachable();
    }
#endif


    return { x, y };
}


[[nodiscard]] bool utils::is_event_in(const Window* window, const SDL_Event& event, const Rect& rect) {

    const auto& [x, y] = get_raw_coordinates(window, event);

    const auto rect_start_x = rect.top_left.x;
    const auto rect_start_y = rect.top_left.y;
    const auto rect_end_x = rect.bottom_right.x;
    const auto rect_end_y = rect.bottom_right.y;


    const bool button_clicked = (x >= rect_start_x and x <= rect_end_x and y >= rect_start_y and y <= rect_end_y);

    return button_clicked;
}
