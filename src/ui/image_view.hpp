#pragma once

#include "graphics/renderer.hpp"
#include "graphics/text.hpp"
#include "hoverable.hpp"
#include "ui/widget.hpp"

#include <string>

namespace ui {
    struct ImageView final : public Widget {
    private:
        Texture m_image;
        Rect m_fill_rect;

    public:
        explicit ImageView(
                ServiceProvider* service_provider,
                const std::string& image_path,
                std::pair<double, double> size,
                Alignment alignment,
                const Layout& layout
        )
            : Widget{ layout },
              m_image{ std::move(service_provider->renderer().load_image(image_path)) },
              m_fill_rect{ ui::get_rectangle_aligned(
                      layout,
                      static_cast<u32>(size.first * layout.get_rect().width()),
                      static_cast<u32>(size.second * layout.get_rect().height()),
                      alignment
              ) } { }

        void render(const ServiceProvider& service_provider) const override {

            service_provider.renderer().draw_texture(m_image, m_fill_rect);
        }

        bool handle_event(const SDL_Event&, const Window*) override {

            return false;
        }
    };
} // namespace ui
