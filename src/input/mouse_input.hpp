#pragma once

#include "helper/export_symbols.hpp"
#include "input.hpp"

namespace input {

    struct MouseInput : public PointerInput {
    public:
        OOPETRIS_GRAPHICS_EXPORTED MouseInput();

        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED std::optional<NavigationEvent> get_navigation_event(
                const SDL_Event& event
        ) const override;

        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED std::string describe_navigation_event(NavigationEvent event
        ) const override;

        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED std::optional<PointerEventHelper> get_pointer_event(
                const SDL_Event& event
        ) const override;

        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED SDL_Event
        offset_pointer_event(const SDL_Event& event, const shapes::IPoint& point) const override;
    };


} // namespace input
