#pragma once

#include "font.hpp"
#include <SDL_ttf.h>
#include <filesystem>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

template<typename Resource, typename Derived>
struct ResourceManager {
protected:
    std::unordered_map<std::string, Resource> m_resources;

public:
    const Resource& get(const std::string& key) const {
        return m_resources.at(key);
    }
};

struct FontManager : public ResourceManager<Font, FontManager> {
    void load(const std::string& key, const std::filesystem::path& path, int font_size) {
        m_resources[key] = Font{ path, font_size };
    }
};
