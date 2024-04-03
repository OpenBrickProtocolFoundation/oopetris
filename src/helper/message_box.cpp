
#include "message_box.hpp"

#include <SDL.h>
#include <fmt/format.h>

void helper::MessageBox::show_simple(
        Type type,
        const std::string& title,
        const std::string& content,
        SDL_Window* window
) {

    Uint32 flags = type == Type::Error     ? SDL_MESSAGEBOX_ERROR
                   : type == Type::Warning ? SDL_MESSAGEBOX_WARNING
                                           : SDL_MESSAGEBOX_INFORMATION;

    int result = SDL_ShowSimpleMessageBox(flags, title.c_str(), content.c_str(), window);

    if (result < 0) {
        throw std::runtime_error{ fmt::format("Failed to show Simple MessageBox: {}", SDL_GetError()) };
    }
}
