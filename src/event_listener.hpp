#pragma once

#include <SDL.h>

#include <window.hpp>

struct EventListener {
    virtual ~EventListener() = default;

    virtual void handle_event(const SDL_Event& event, const Window* window) = 0;
};
