#pragma once

#include <core/helper/types.hpp>

#include "focus_layout.hpp"
#include "graphics/renderer.hpp"
#include "ui/focusable.hpp"
#include "ui/hoverable.hpp"
#include "ui/layouts/grid_layout.hpp"

#include <vector>


// this is needed for some hacks regarding scroll layout focus switches
namespace scenes {
    struct OnlineLobby;
}
namespace ui {

    struct TileLayout : public FocusLayout {
    private:
        u32 size;
        Direction direction;
        std::vector<double> steps;
        Margin gap;
        std::pair<u32, u32> margin;

    public:
        // see here, why utils::size_t_identity<S> is needed: https://stackoverflow.com/questions/2786946/c-invoke-explicit-template-constructor
        template<size_t S>
        explicit TileLayout(
                utils::size_t_identity<S>,
                u32 focus_id,
                Direction direction,
                std::array<double, S - 1> steps,
                Margin gap,
                std::pair<double, double> margin,
                const Layout& layout,
                bool is_top_level = true
        )
            : FocusLayout{ layout, focus_id,FocusOptions{ is_top_level, is_top_level }, is_top_level },
              size{ S },
              direction{ direction },
              steps{ steps.cbegin(), steps.cend() },
              gap{ gap },
              margin{ static_cast<u32>(margin.first * layout.get_rect().width()),
                      static_cast<u32>(margin.second * layout.get_rect().height()) } {
            static_assert(S != 0 and "TileLayout has to hold at least one child");
        }

        OOPETRIS_GRAPHICS_EXPORTED void render(const ServiceProvider& service_provider) const override;

    private:
        [[nodiscard]] Layout get_layout_for_index(u32 index) override;
    };

} // namespace ui
