

#pragma once

#if not defined(__SWITCH__)
#error "this header is switch only"
#endif

#include "input.hpp"
#include "manager/event_dispatcher.hpp"
#include "switch_buttons.hpp"

struct JoystickInput : public Input, public EventListener {
private:
    std::vector<SDL_Event> m_event_buffer;
    EventDispatcher* m_event_dispatcher;

public:
    JoystickInput(EventDispatcher* event_dispatcher)
        : Input{ InputType::Controller },
          m_event_dispatcher{ event_dispatcher } {
        m_event_dispatcher->register_listener(this);
    }

    ~JoystickInput() override {
        m_event_dispatcher->unregister_listener(this);
    }

    void handle_event(const SDL_Event& event, const Window* window) override;

    void update(SimulationStep simulation_step_index) override;

private:
    [[nodiscard]] helper::optional<InputEvent> sdl_event_to_input_event(const SDL_Event& event) const;
};
