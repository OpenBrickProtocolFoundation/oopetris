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

    //TODO(Totto):  factor out to some other place!
    std::vector<sdl::Key> m_allowed_input_keys{
        sdl::Key{ SDLK_RETURN },
        sdl::Key{ SDLK_BACKSPACE },
        sdl::Key{ SDLK_BACKSPACE, { sdl::Modifier::CTRL } },
        sdl::Key{ SDLK_DOWN },
        sdl::Key{ SDLK_UP },
        sdl::Key{ SDLK_LEFT },
        sdl::Key{ SDLK_RIGHT },
        sdl::Key{ SDLK_ESCAPE },
        sdl::Key{ SDLK_TAB },
        sdl::Key{ SDLK_c, { sdl::Modifier::CTRL } },
        sdl::Key{ SDLK_v, { sdl::Modifier::CTRL } }
    };

public:
    explicit EventDispatcher() = default;

    void register_listener(EventListener* listener) {
        m_listeners.push_back(listener);
    }

    void unregister_listener(const EventListener* listener) {
        const auto end = std::ranges::remove(m_listeners, listener).begin();
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
                        if (std::ranges::find(m_allowed_input_keys, sdl::Key{ event.key.keysym })
                            == m_allowed_input_keys.cend()) {
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
