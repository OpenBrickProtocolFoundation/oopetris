#pragma once

#include <SDL_ttf.h>
#include <filesystem>
#include <memory>
#include <string>

struct FontLoadingError final : public std::exception {
private:
    std::string message;

public:
    explicit FontLoadingError(std::string message) : message{ std::move(message) } { }

    [[nodiscard]] const char* what() const noexcept override {
        return message.c_str();
    }
};


struct Font final {
private:
    std::shared_ptr<TTF_Font> m_font;

public:
    Font() = default;
    Font(const std::filesystem::path& path, int size);

    friend struct Text;
};
