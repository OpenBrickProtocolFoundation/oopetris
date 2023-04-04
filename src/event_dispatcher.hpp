#pragma once

#include "event_listener.hpp"
#include <SDL.h>
#include <memory>
#include <vector>

struct EventDispatcher final {
private:
    std::vector<EventListener*> m_listeners;

public:
    void register_listener(EventListener* listener) {
        m_listeners.push_back(listener);
    }

    void dispatch_pending_events() const {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            for (const auto& listener : m_listeners) {
                listener->handle_event(event);
            }
        }
    }
};
