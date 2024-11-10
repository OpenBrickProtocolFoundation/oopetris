

#pragma once

#include <core/helper/types.hpp>

#include "helper/windows.hpp"
#include "ui/layout.hpp"
#include <vector>

namespace game {
    OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] std::vector<ui::Layout>
    get_layouts_for(std::size_t players, const ui::Layout& layout);

}
