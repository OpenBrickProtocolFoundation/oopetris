#pragma once

#include <SDL_ttf.h>
#include <filesystem>
#include <memory>
#include <string>

#include "helper/windows.hpp"

struct FontLoadingError final : public std::exception {
private:
    std::string m_message;

public:
    OOPETRIS_GRAPHICS_EXPORTED explicit FontLoadingError(std::string message) : m_message{ std::move(message) } { }

    [[nodiscard]] const char* what() const noexcept override {
        return m_message.c_str();
    }
};


struct Font final {
private:
    std::shared_ptr<TTF_Font> m_font;

public:
    Font() = default;
    OOPETRIS_GRAPHICS_EXPORTED Font(const std::filesystem::path& path, int size);

    OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] TTF_Font* get() const;

    friend struct Text;
    friend struct Renderer;
};
