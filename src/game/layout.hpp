

#pragma once

#include <core/helper/types.hpp>

#include "helper/windows.hpp"
#include "ui/layout.hpp"


namespace game {
    OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] std::vector<ui::Layout>
    get_layouts_for(u32 players, const ui::Layout& layout);

}
