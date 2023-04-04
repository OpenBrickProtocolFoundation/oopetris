#pragma once

#include <SDL.h>

struct EventListener {
    virtual ~EventListener() = default;

    virtual void handle_event(const SDL_Event& event) = 0;
};
