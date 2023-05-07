#include "window.hpp"
#include <stdexcept>
#include <string>

Window::Window(const std::string& title, WindowPosition position, int width, int height)
    : Window{ title, static_cast<int>(position), static_cast<int>(position), width, height } { }

Window::Window(const std::string& title, int x, int y, int width, int height)
    : m_window{ SDL_CreateWindow(title.c_str(), x, y, width, height, 0) } { }

Window::Window(const std::string& title, WindowPosition position)
    : Window{ title, static_cast<int>(position), static_cast<int>(position) } { }

Window::Window(const std::string& title, int x, int y) {

    SDL_DisplayMode mode{};
    const int result = SDL_GetCurrentDisplayMode(0, &mode);
    if (result != 0) {
        throw std::runtime_error{ "failed in getting display mode: " + std::string{ SDL_GetError() } };
    }
    m_window = SDL_CreateWindow(title.c_str(), x, y, mode.w, mode.h, 0);
    if (m_window == nullptr) {
        throw std::runtime_error{ "failed in creating window: " + std::string{ SDL_GetError() } };
    }
}


Window::~Window() {
    SDL_DestroyWindow(m_window);
}

[[nodiscard]] Point Window::size() const {
    int width;
    int height;
    SDL_GetWindowSize(m_window, &width, &height);
    return Point{ width, height };
}

[[nodiscard]] SDL_Window* Window::get_sdl_window() const {
    return m_window;
}

[[nodiscard]] Rect Window::screen_rect() const {
    const auto window_size = size();
    return Rect{ 0, 0, window_size.x, window_size.y };
}
