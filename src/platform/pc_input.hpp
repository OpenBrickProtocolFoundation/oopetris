#pragma once

#include "input.hpp"
#include "manager/event_dispatcher.hpp"

struct KeyboardInput : public Input, public EventListener {
private:
    KeyboardControls m_controls;
    std::vector<SDL_Event> m_event_buffer;
    EventDispatcher* m_event_dispatcher;

public:
    KeyboardInput(KeyboardControls controls, EventDispatcher* event_dispatcher);

    ~KeyboardInput() override;

    void handle_event(const SDL_Event& event, const Window* window) override;

    void update(SimulationStep simulation_step_index) override;

private:
    [[nodiscard]] helper::optional<InputEvent> sdl_event_to_input_event(const SDL_Event& event) const;
};
