

#include "mouse_input.hpp"


[[nodiscard]] SDL_Event input::MouseInput::offset_pointer_event(const SDL_Event& event, const shapes::IPoint& point)
        const {

    auto new_event = event;

    switch (event.type) {
        case SDL_MOUSEMOTION:
            new_event.motion.x = event.motion.x + point.x;
            new_event.motion.y = event.motion.y + point.y;
            break;
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
            new_event.button.x = event.button.x + point.x;
            new_event.button.y = event.button.y + point.y;
            break;
        default:
            throw std::runtime_error("Tried to offset event, that is no pointer event: in Mouse Input");
    }

    return new_event;
}


//TODO:
/* 
[[nodiscard]] bool utils::event_is_click_event(const SDL_Event& event, CrossPlatformClickEvent click_type) {


    decltype(event.type) desired_type{};
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
            return event.type == SDL_MOUSEMOTION
                   || ((event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP)
                       && event.button.button == SDL_BUTTON_LEFT);
        default:
            utils::unreachable();
    }


    return event.type == desired_type && event.button.button == SDL_BUTTON_LEFT;
 */


/**


[[nodiscard]] std::pair<i32, i32> utils::get_raw_coordinates(const Window* window, const SDL_Event& event) {

    assert(utils::event_is_click_event(event, utils::CrossPlatformClickEvent::Any) && "expected a click event");

#if defined(__ANDROID__)
    // These are doubles, from 0-1 (or if using virtual layouts > 0) in percent, the have to be casted to absolut x coordinates!
    const double x_percent = event.tfinger.x;
    const double y_percent = event.tfinger.y;
    const auto window_size = window->size();
    const auto x = static_cast<i32>(std::round(x_percent * window_size.x));
    const auto y = static_cast<i32>(std::round(y_percent * window_size.y));


#elif defined(__SWITCH__)
    UNUSED(window);
    UNUSED(event);
    throw std::runtime_error("Not supported on the Nintendo switch");
    int x{};
    int y{};
#else
    UNUSED(window);

    Sint32 x{};
    Sint32 y{};
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


    return { static_cast<i32>(x), static_cast<i32>(y) };
}

 */
