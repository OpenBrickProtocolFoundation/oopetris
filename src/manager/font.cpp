#include "manager/font.hpp"
#include <filesystem>
#include <string>

Font::Font(const std::filesystem::path& path, int size)
    : m_font{ TTF_OpenFont(path.string().c_str(), size), TTF_CloseFont } {
    if (m_font == nullptr) {
        // this error is bogus if the file doesn't exist, don't ask me why
        std::string error = std::string{ TTF_GetError() };
        if (not std::filesystem::exists(path)) {
            error = "path '" + path.string() + "' doesn't exist!";
        }
        throw FontLoadingError{ "error loading font: '" + error + "'" };
    }
}

TTF_Font* Font::get() const {
    return m_font.get();
}
