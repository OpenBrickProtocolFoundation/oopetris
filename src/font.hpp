#pragma once

#include <SDL_ttf.h>
#include <string>

struct FontLoadingError final : public std::exception {
private:
    std::string message;

public:
    explicit FontLoadingError(std::string message) : message{ message } { }

    const char* what() const noexcept override {
        return message.c_str();
    }
};

struct Font final {
private:
    TTF_Font* m_font;

public:
    Font(const std::string& path, int size);
    Font(const Font&) = delete;
    Font& operator=(const Font&) = delete;
    ~Font();

    friend struct Text;
};
