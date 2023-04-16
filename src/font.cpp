#include "font.hpp"
#include <string>

Font::Font(const std::filesystem::path& path, int size) {
    m_font = TTF_OpenFont(path.string().c_str(), size);
    if (m_font == nullptr) {
        throw FontLoadingError{ "error loading font: '" + std::string{ TTF_GetError() } + "'" };
    }
}

Font::~Font() {
    if (m_font != nullptr) {
        TTF_CloseFont(m_font);
    }
}
