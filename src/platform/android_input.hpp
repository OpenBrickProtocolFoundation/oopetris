#pragma once

#if not defined(__ANDROID__)
#error "this header is android only"
#endif

#include "input.hpp"
#include "manager/event_dispatcher.hpp"


struct TouchInput final : public Input, public EventListener {
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
    explicit TouchInput(EventDispatcher* event_dispatcher)
        : Input{ InputType::Touch },
          m_event_dispatcher{ event_dispatcher } {
        m_event_dispatcher->register_listener(this);
    }

    ~TouchInput() override {
        m_event_dispatcher->unregister_listener(this);
    }

    void handle_event(const SDL_Event& event, const Window* window) override;
    void update(SimulationStep simulation_step_index) override;

private:
    [[nodiscard]] helper::optional<InputEvent> sdl_event_to_input_event(const SDL_Event& event);
};
