#pragma once

#include "helper/types.hpp"
#include "manager/font.hpp"

#include <SDL_ttf.h>
#include <filesystem>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

enum class FontId : u8 { Default, Arial, NotoColorEmoji, Symbola };

template<typename Key, typename Resource>
struct ResourceManager {
private:
    std::unordered_map<Key, Resource> m_resources;

protected:
    ResourceManager() = default;

    [[nodiscard]] const std::unordered_map<Key, Resource>& resources() const {
        return m_resources;
    }

    [[nodiscard]] std::unordered_map<Key, Resource>& resources() {
        return m_resources;
    }

public:
    [[nodiscard]] const Resource& get(Key key) const {
        return m_resources.at(key);
    }
};

struct FontManager : public ResourceManager<FontId, Font> {
    void load(FontId key, const std::filesystem::path& path, int font_size) {
        resources()[key] = Font{ path, font_size };
    }
};
