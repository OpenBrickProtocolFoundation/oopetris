#pragma once

#include "../types.hpp"
#include <variant>

namespace ui {
    struct AbsoluteLayout {
        u32 x;
        u32 y;

        AbsoluteLayout(const u32 x, const u32 y) : x{ x }, y{ y } { }
    };

    /*struct GridLayout {
        u32 x;
        u32 y;
        u32 width;
        u32 height;

        GridLayout(const u32 x, const u32 y, const u32 width = 1, const u32 height = 1)
            : x{ x },
              y{ y },
              width{ width },
              height{ height } { }
    };*/

    using Layout = std::variant<AbsoluteLayout /*, GridLayout*/>;
} // namespace ui
