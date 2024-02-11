#pragma once

#include "input/event_listener.hpp"
#include <SDL.h>
#include <algorithm>
#include <cassert>
#include <memory>
#include <vector>

struct EventDispatcher final {
private:
    std::vector<EventListener*> m_listeners;
    Window* m_window;

public:
    EventDispatcher(Window* window) : m_window{ window } {};

    void register_listener(EventListener* listener) {
        m_listeners.push_back(listener);
    }

    void unregister_listener(const EventListener* listener) {
        const auto end = std::remove(m_listeners.begin(), m_listeners.end(), listener);
        assert(end != m_listeners.end() and "listener to delete could not be found");
        m_listeners.erase(end, m_listeners.end());
    }

    void dispatch_pending_events() const {
        SDL_Event event;
        while (SDL_PollEvent(&event) != 0) {
            for (const auto& listener : m_listeners) {
                listener->handle_event(event, m_window);
            }
        }
    }
};
