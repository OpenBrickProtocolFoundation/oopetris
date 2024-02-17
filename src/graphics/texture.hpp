

#include "helper/color.hpp"
#include "manager/font.hpp"
#include "point.hpp"
#include "rect.hpp"

#include <SDL.h>
#include <SDL_image.h>
#include <spdlog/spdlog.h>
#include <string>

struct Texture {
private:
    SDL_Texture* m_raw_texture;

    explicit Texture(SDL_Texture* raw_texture) : m_raw_texture{ raw_texture } { }

public:
    static Texture from_image(SDL_Renderer* renderer, const std::string& image_path) {
        SDL_Texture* image = IMG_LoadTexture(renderer, image_path.c_str());

        if (image == nullptr) {
            spdlog::error("Failed to load image from path '{}' with error: {}", image_path, SDL_GetError());
        }
        return Texture{ image };
    }

    static Texture
    prerender_text(SDL_Renderer* renderer, const std::string& text, const Font& font, const Color& color) {

        const SDL_Color text_color = color.to_sdl_color();
        SDL_Surface* const surface = TTF_RenderUTF8_Solid(font.get(), text.c_str(), text_color);
        if (surface == nullptr) {
            spdlog::error("Failed to pre-render text with error: {}", SDL_GetError());
        }

        SDL_Texture* const texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);


        return Texture{ texture };
    }

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    Texture(Texture&& old) noexcept : m_raw_texture{ old.m_raw_texture } {
        old.m_raw_texture = nullptr;
    };

    Texture& operator=(Texture&& other) noexcept {
        if (this != &other) {
            this->~Texture();
            this->m_raw_texture = other.m_raw_texture;
            other.m_raw_texture = nullptr;
        }

        return *this;
    };

    ~Texture() {

        if (m_raw_texture != nullptr) {
            SDL_DestroyTexture(m_raw_texture);
            m_raw_texture = nullptr;
        }
    }

    void render(SDL_Renderer* renderer, const shapes::Rect& rect) const {
        const SDL_Rect rect_sdl = rect.to_sdl_rect();
        SDL_RenderCopy(renderer, m_raw_texture, nullptr, &rect_sdl);
    }

    [[nodiscard]] shapes::Point size() const {
        shapes::Point size;
        const auto result = SDL_QueryTexture(m_raw_texture, nullptr, nullptr, &size.x, &size.y);

        if (result < 0) {
            spdlog::error("Failed to get texture size with error: {}", SDL_GetError());
            return { 0, 0 };
        }
        return size;
    }
};
