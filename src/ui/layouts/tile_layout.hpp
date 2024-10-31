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
        u32 m_size;
        Direction m_direction;
        std::vector<double> m_steps;
        Margin m_gap;
        std::pair<u32, u32> m_margin;

    public:
        // see here, why utils::SizeIdentity<S> is needed: https://stackoverflow.com/questions/2786946/c-invoke-explicit-template-constructor
        template<size_t S>
        explicit TileLayout(
                utils::SizeIdentity<S> /*unused*/,
                u32 focus_id,
                Direction direction,
                std::array<double, S - 1> steps,
                Margin gap,
                std::pair<double, double> margin,
                const Layout& layout,
                bool is_top_level = true
        )
            : FocusLayout{ layout, focus_id,FocusOptions{ .wrap_around=is_top_level, .allow_tab=is_top_level }, is_top_level },
              m_size{ S },
              m_direction{ direction },
              m_steps{ steps.cbegin(), steps.cend() },
              m_gap{ gap },
              m_margin{ static_cast<u32>(margin.first * layout.get_rect().width()),
                      static_cast<u32>(margin.second * layout.get_rect().height()) } {
            static_assert(S != 0 and "TileLayout has to hold at least one child");
        }

        OOPETRIS_GRAPHICS_EXPORTED void render(const ServiceProvider& service_provider) const override;

    private:
        [[nodiscard]] Layout get_layout_for_index(u32 index) override;
    };

} // namespace ui
