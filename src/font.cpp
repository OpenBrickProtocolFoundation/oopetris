#include "font.hpp"

Font::Font(const std::string& path, int size) {
    m_font = TTF_OpenFont(path.c_str(), size);
    if (m_font == nullptr) {
        throw FontLoadingError{ "error loading font" };
    }
}

Font::~Font() {
    if (m_font != nullptr) {
        TTF_CloseFont(m_font);
    }
}
