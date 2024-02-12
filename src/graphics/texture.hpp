

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
            spdlog::error("Failed to load image from path '{}'", image_path);
        }
        return Texture{ image };
    }

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    Texture(Texture&& old) : m_raw_texture{ old.m_raw_texture } {
        old.m_raw_texture = nullptr;
    };

    Texture operator=(Texture&& old) {
        return Texture{ std::move(old) };
    };

    ~Texture() {
        if (m_raw_texture != nullptr) {
            SDL_DestroyTexture(m_raw_texture);
        }
    }

    void render(SDL_Renderer* renderer, const Rect& rect) const {
        const SDL_Rect rect_sdl = rect.to_sdl_rect();
        SDL_RenderCopy(renderer, m_raw_texture, nullptr, &rect_sdl);
    }
};
