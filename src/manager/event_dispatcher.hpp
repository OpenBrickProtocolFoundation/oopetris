#pragma once

#include "graphics/rect.hpp"
#include "helper/optional.hpp"
#include "manager/event_listener.hpp"

#include <SDL.h>
#include <algorithm>
#include <cassert>
#include <vector>

struct EventDispatcher final {
private:
    std::vector<EventListener*> m_listeners;
    Window* m_window;
    bool m_input_activated{ false };
    bool m_enabled{ true };
    std::vector<SDL_Keycode> allowed_input_keys{ SDLK_RETURN, SDLK_BACKSPACE, SDLK_DOWN,   SDLK_UP,
                                                 SDLK_LEFT,   SDLK_RIGHT,     SDLK_ESCAPE, SDLK_TAB };

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
        if (not m_enabled) {
            return;
        }

        SDL_Event event;
        while (SDL_PollEvent(&event) != 0) {

            if (m_input_activated) {
                switch (event.type) {
                    case SDL_KEYDOWN:
                    case SDL_KEYUP: {
                        if (event.key.keysym.sym == SDLK_v and (event.key.keysym.mod & KMOD_CTRL) != 0) {
                            break;
                        }
                        if (std::find(allowed_input_keys.cbegin(), allowed_input_keys.cend(), event.key.keysym.sym)
                            == allowed_input_keys.cend()) {
                            return;
                        }

                        break;
                    }
                    default:
                        break;
                }
            }

            for (const auto& listener : m_listeners) {
                if (listener->is_paused()) {
                    continue;
                }

                listener->handle_event(event, m_window);
            }
        }
    }

    void start_text_input(const helper::optional<shapes::URect>& rect) {
        if (m_input_activated) {
            return;
        }

        if (rect.has_value()) {
            SDL_Rect sdl_rect = rect.value().to_sdl_rect();
            SDL_SetTextInputRect(&sdl_rect);
        }

        SDL_StartTextInput();

        m_input_activated = true;
    }

    void stop_text_input() {
        if (not m_input_activated) {
            return;
        }

        SDL_StopTextInput();

        m_input_activated = false;
    }

    void disable() {
        m_enabled = false;
    }

    static void clear_all_events() {
        SDL_PumpEvents();
        SDL_FlushEvents(SDL_USEREVENT, SDL_LASTEVENT);
    }

    void enable() {
        if (not m_enabled) {
            // clear events received in the phase after disabling it
            clear_all_events();
        }

        m_enabled = true;
    }

    [[nodiscard]] bool is_enabled() const {
        return m_enabled;
    }
};
