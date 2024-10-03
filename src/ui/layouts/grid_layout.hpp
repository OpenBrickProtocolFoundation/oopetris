#pragma once

#include <core/helper/types.hpp>

#include "focus_layout.hpp"

namespace ui {
    struct GridLayout : public FocusLayout {
    private:
        u32 size;
        Direction direction;
        Margin gap;
        std::pair<u32, u32> margin;

    public:
        OOPETRIS_GRAPHICS_EXPORTED explicit GridLayout(
                u32 focus_id,
                u32 size,
                Direction direction,
                Margin gap,
                std::pair<double, double> margin,
                const Layout& layout,
                bool is_top_level = true
        );

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] u32 total_size() const;

        OOPETRIS_GRAPHICS_EXPORTED void render(const ServiceProvider& service_provider) const override;

    private:
        [[nodiscard]] Layout get_layout_for_index(u32 index) override;
    };

} // namespace ui
