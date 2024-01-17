#include "window.hpp"
#include <stdexcept>
#include <string>

Window::Window(const std::string& title, WindowPosition position, int width, int height)
    : Window{ title, static_cast<int>(position), static_cast<int>(position), width, height } { }
#ifdef _USE_SDL_LEGACY_VERSION
Window::Window(
        const std::string& title,
        [[maybe_unused]] int x,
        [[maybe_unused]] int y,
        [[maybe_unused]] int width,
        [[maybe_unused]] int height
) {
    // title, icon name
    SDL_WM_SetCaption(title.c_str(), title.c_str());

    //TODO: this is not entirely correct, but not supported in sdl < 2
    m_screen = SDL_SetVideoMode(width, height, 0, 0);
}
#else
Window::Window(const std::string& title, int x, int y, int width, int height)
    : m_window{ SDL_CreateWindow(title.c_str(), x, y, width, height, 0) } { }
#endif
Window::Window(const std::string& title, WindowPosition position)
    : Window{ title, static_cast<int>(position), static_cast<int>(position) } { }

Window::Window(const std::string& title, int x, int y) {
#ifdef _USE_SDL_LEGACY_VERSION
    // title, icon name
    SDL_WM_SetCaption(title.c_str(), title.c_str());

    //TODO: this is not entirely correct!
    m_screen = SDL_SetVideoMode(x, y, 0, 0);
#else

    SDL_DisplayMode mode{};
    const int result = SDL_GetCurrentDisplayMode(0, &mode);
    if (result != 0) {
        throw std::runtime_error{ "failed in getting display mode: " + std::string{ SDL_GetError() } };
    }
    m_window = SDL_CreateWindow(title.c_str(), x, y, mode.w, mode.h, 0);
    if (m_window == nullptr) {
        throw std::runtime_error{ "failed in creating window: " + std::string{ SDL_GetError() } };
    }
#endif
}


Window::~Window() {
#ifdef _USE_SDL_LEGACY_VERSION
    SDL_FreeSurface(m_screen);
#else
    SDL_DestroyWindow(m_window);
#endif
}

[[nodiscard]] Point Window::size() const {
    SDL_int width{};
    SDL_int height{};
#ifdef _USE_SDL_LEGACY_VERSION
    width = m_screen->w;
    height = m_screen->h;
#else
    SDL_GetWindowSize(m_window, &width, &height);
#endif

    return Point{ width, height };
}

#ifdef _USE_SDL_LEGACY_VERSION
[[nodiscard]] SDL_Surface* Window::get_sdl_screen() const {
    return m_screen;
}
#else
[[nodiscard]] SDL_Window* Window::get_sdl_window() const {
    return m_window;
}
#endif


[[nodiscard]] Rect Window::screen_rect() const {
    const auto window_size = size();
    return Rect{ 0, 0, window_size.x, window_size.y };
}
