#pragma once


#include "input.hpp"
#include "input/game_input.hpp"
#include "manager/event_dispatcher.hpp"


namespace input {

    struct TouchInput : PointerInput {
        std::shared_ptr<Window> m_window;

    public:
        TouchInput(const std::shared_ptr<Window>& window);
        virtual ~TouchInput();


        [[nodiscard]] helper::optional<NavigationEvent> get_navigation_event(const SDL_Event& event) const override;

        [[nodiscard]] helper::optional<PointerEventHelper> get_pointer_event(const SDL_Event& event) const override;

        [[nodiscard]] std::string describe_navigation_event(NavigationEvent event) const override;
    };

    struct TouchGameInput final : public GameInput, public EventListener {
    private:
        struct PressedState {
            Uint32 timestamp;
            float x;
            float y;
            explicit PressedState(Uint32 timestamp, float x, float y) : timestamp{ timestamp }, x{ x }, y{ y } { }
        };

        std::unordered_map<SDL_FingerID, helper::optional<PressedState>> m_finger_state;
        std::vector<SDL_Event> m_event_buffer;
        EventDispatcher* m_event_dispatcher;

    public:
        explicit TouchGameInput(EventDispatcher* event_dispatcher)
            : GameInput{ GameInputType::Touch },
              m_event_dispatcher{ event_dispatcher } {
            m_event_dispatcher->register_listener(this);
        }

        ~TouchGameInput() override {
            m_event_dispatcher->unregister_listener(this);
        }

        void handle_event(const SDL_Event& event) override;
        void update(SimulationStep simulation_step_index) override;

        [[nodiscard]] helper::optional<MenuEvent> get_menu_event(const SDL_Event& event) const override;

        [[nodiscard]] std::string describe_menu_event(MenuEvent event) const override;

    private:
        [[nodiscard]] helper::optional<InputEvent> sdl_event_to_input_event(const SDL_Event& event);
    };

} // namespace input


//TODO:
/* 


    decltype(event.type) desired_type{};
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

    return event.type == desired_type;
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


 * 
 */


/* 

[[nodiscard]] SDL_Event utils::offset_event(const Window* window, const SDL_Event& event, const shapes::IPoint& point) {


    assert(utils::event_is_click_event(event, utils::CrossPlatformClickEvent::Any) && "expected a click event");


    auto new_event = event;

#if defined(__ANDROID__)
    // These are doubles in percent, the have to be modified by using the windows sizes


    const double x_percent = event.tfinger.x;
    const double y_percent = event.tfinger.y;
    const auto window_size = window->size();
    new_event.tfinger.x = x_percent + static_cast<double>(point.x) / static_cast<double>(window_size.x);
    new_event.tfinger.y = y_percent + static_cast<double>(point.y) / static_cast<double>(window_size.y);


#elif defined(__SWITCH__)
    UNUSED(window);
    UNUSED(event);
    UNUSED(point);
    UNUSED(new_event);
    throw std::runtime_error("Not supported on the Nintendo switch");
#else
    UNUSED(window);

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
            utils::unreachable();
    }
#endif


    return new_event;
}
 */
