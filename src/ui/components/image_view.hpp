#pragma once

#include "graphics/renderer.hpp"
#include "graphics/text.hpp"
#include "ui/hoverable.hpp"
#include "ui/widget.hpp"

#include <filesystem>
#include <string>

namespace ui {
    struct ImageView final : public Widget {
    private:
        Texture m_image;
        shapes::URect m_fill_rect;

    public:
        OOPETRIS_GRAPHICS_EXPORTED explicit ImageView(
                ServiceProvider* service_provider,
                const std::filesystem::path& image_path,
                std::pair<double, double> size,
                bool respect_ratio,
                Alignment alignment,
                const Layout& layout,
                bool is_top_level
        );

        OOPETRIS_GRAPHICS_EXPORTED void render(const ServiceProvider& service_provider) const override;

        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED Widget::EventHandleResult
        handle_event(const std::shared_ptr<input::InputManager>& input_manager, const SDL_Event& even) override;
    };
} // namespace ui
