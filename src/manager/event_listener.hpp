#pragma once

#include <SDL.h>

#include "graphics/window.hpp"

struct EventListener {
    bool m_is_paused{ false };

    virtual ~EventListener() = default;

    virtual void handle_event(const SDL_Event& event, const Window* window) = 0;

    [[nodiscard]] bool is_paused() const {
        return m_is_paused;
    }

    void set_paused(bool is_paused) {
        m_is_paused = is_paused;
    }
};
