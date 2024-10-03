#pragma once

#include "helper/windows.hpp"
#include "input.hpp"

namespace input {

    struct MouseInput : public PointerInput {
    public:
        OOPETRIS_GRAPHICS_EXPORTED MouseInput();

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] std::optional<NavigationEvent> get_navigation_event(
                const SDL_Event& event
        ) const override;

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] std::string describe_navigation_event(NavigationEvent event
        ) const override;

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] std::optional<PointerEventHelper> get_pointer_event(
                const SDL_Event& event
        ) const override;

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] SDL_Event
        offset_pointer_event(const SDL_Event& event, const shapes::IPoint& point) const override;
    };


} // namespace input
