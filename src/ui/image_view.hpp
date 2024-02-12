#pragma once

#include "graphics/text.hpp"
#include "hoverable.hpp"
#include "ui/widget.hpp"

#include <SDL_image.h>
#include <spdlog/spdlog.h>
#include <string>

namespace ui {
    struct ImageView final : public Widget {
    private:
        SDL_Surface* m_image;
        Rect m_fill_rect;


    public:
        explicit ImageView(
                const std::string& image_path,
                std::pair<double, double> size,
                Alignment alignment,
                const Layout& layout
        )
            : Widget{ layout },
              m_fill_rect{ ui::get_rectangle_aligned(
                      layout,
                      static_cast<u32>(size.first * layout.get_rect().width()),
                      static_cast<u32>(size.second * layout.get_rect().height()),
                      alignment
              ) } {


            m_image = IMG_Load(image_path.c_str());

            if (m_image == nullptr) {
                spdlog::error("Failed to load image from path '{}'", image_path);
            }
        }

        void render(const ServiceProvider& service_provider) const override {
            //TODO:
            (void)service_provider;   
        }

        bool handle_event(const SDL_Event&, const Window*) override {


            return false;
        }
    };
} // namespace ui
