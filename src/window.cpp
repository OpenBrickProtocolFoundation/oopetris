#include "window.hpp"

Window::Window(const std::string& title, WindowPosition position, int width, int height)
    : Window{ title, static_cast<int>(position), static_cast<int>(position), width, height } { }

Window::Window(const std::string& title, int x, int y, int width, int height)
    : m_window{ SDL_CreateWindow(title.c_str(), x, y, width, height, 0) } { }

Window::~Window() {
    SDL_DestroyWindow(m_window);
}

Point Window::size() const {
    int width;
    int height;
    SDL_GetWindowSize(m_window, &width, &height);
    return Point{ width, height };
}

SDL_Window* Window::get_sdl_window() const {
    return m_window;
}
