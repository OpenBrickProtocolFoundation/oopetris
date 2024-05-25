#pragma once

#include "input.hpp"


namespace input {

    struct MouseInput : public PointerInput {
    public:
        MouseInput();

        [[nodiscard]] std::optional<NavigationEvent> get_navigation_event(const SDL_Event& event) const override;

        [[nodiscard]] std::string describe_navigation_event(NavigationEvent event) const override;

        [[nodiscard]] std::optional<PointerEventHelper> get_pointer_event(const SDL_Event& event) const override;

        [[nodiscard]] SDL_Event offset_pointer_event(const SDL_Event& event, const shapes::IPoint& point)
                const override;
    };


} // namespace input
