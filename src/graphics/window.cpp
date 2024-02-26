#include "graphics/window.hpp"
#include <stdexcept>
#include <string>

Window::Window(const std::string& title, WindowPosition position, u32 width, u32 height)
    : Window{ title, static_cast<u32>(position), static_cast<u32>(position), width, height } { }

Window::Window(const std::string& title, u32 x, u32 y, u32 width, u32 height)
    : m_window{ SDL_CreateWindow(title.c_str(), x, y, width, height, 0) } { }

Window::Window(const std::string& title, WindowPosition position)
    : Window{ title, static_cast<u32>(position), static_cast<u32>(position) } { }

Window::Window(const std::string& title, u32 x, u32 y) {

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

[[nodiscard]] shapes::UPoint Window::size() const {
    int width{};
    int height{};
    SDL_GetWindowSize(m_window, &width, &height);
    return shapes::UPoint{ static_cast<u32>(width), static_cast<u32>(height) };
}

[[nodiscard]] SDL_Window* Window::get_sdl_window() const {
    return m_window;
}

[[nodiscard]] shapes::URect Window::screen_rect() const {
    const auto window_size = size();
    return shapes::URect{ 0, 0, window_size.x, window_size.y };
}
