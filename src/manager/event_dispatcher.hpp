#pragma once

#include "graphics/rect.hpp"
#include "helper/optional.hpp"
#include "manager/event_listener.hpp"
#include "sdl_key.hpp"

#include <algorithm>
#include <cassert>
#include <vector>

struct EventDispatcher final {
private:
    std::vector<EventListener*> m_listeners;
    bool m_input_activated{ false };
    bool m_enabled{ true };

    //TODO: factor out to some other place!
    std::vector<SDL::Key> allowed_input_keys{
        SDL::Key{ SDLK_RETURN },
        SDL::Key{ SDLK_BACKSPACE },
        SDL::Key{ SDLK_BACKSPACE, { SDL::Modifier::CTRL } },
        SDL::Key{ SDLK_DOWN },
        SDL::Key{ SDLK_UP },
        SDL::Key{ SDLK_LEFT },
        SDL::Key{ SDLK_RIGHT },
        SDL::Key{ SDLK_ESCAPE },
        SDL::Key{ SDLK_TAB },
        SDL::Key{ SDLK_c, { SDL::Modifier::CTRL } },
        SDL::Key{ SDLK_v, { SDL::Modifier::CTRL } }
    };

public:
    explicit EventDispatcher() = default;

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
                        if (std::find(
                                    allowed_input_keys.cbegin(), allowed_input_keys.cend(), SDL::Key{ event.key.keysym }
                            )
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

                listener->handle_event(event);
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
