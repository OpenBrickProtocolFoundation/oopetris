#pragma once

#include "../types.hpp"
#include "../utils.hpp"
#include "../window.hpp"

#include <cassert>

namespace ui {

    struct Layout {
    private:
        Rect m_rect;

    public:
        Layout(const Rect& rect) : m_rect{ rect } { }


        [[nodiscard]] const Rect& get_rect() const {
            return m_rect;
        }
    };


    struct AbsolutLayout : public Layout {
        AbsolutLayout(const u32 x, const u32 y, const u32 width, const u32 height)
            : Layout{
                  Rect{static_cast<int>(x), static_cast<int>(y), static_cast<int>(width), static_cast<int>(height)}
        } {
            //TODO: assert dimensions
        }
    };


    struct FullScreenLayout : public Layout {
        FullScreenLayout(const Window& window) : Layout{ window.screen_rect() } { }
        FullScreenLayout(const Window* window) : FullScreenLayout{ *window } { }
    };


    struct RelativeLayout : public Layout {
        RelativeLayout(const Window& window, const double x, const double y, const double width, const double height)
            : Layout{
                  Rect{
                       static_cast<int>(x * window.screen_rect().width()),
                       static_cast<int>(y * window.screen_rect().height()),
                       static_cast<int>(width * window.screen_rect().width()),
                       static_cast<int>(height * window.screen_rect().height()),
                       }
        } {
            assert(x >= 0.0 && x <= 1.0 && "x has to be in correct percentage range!");
            assert(y >= 0.0 && y <= 1.0 && "y has to be in correct percentage range!");

            assert(width >= 0.0 && width <= 1.0 && "width has to be in correct percentage range!");
            assert(height >= 0.0 && height <= 1.0 && "height has to be in correct percentage range!");
        }
        RelativeLayout(const Window* window, const double x, const double y, const double width, const double height)
            : RelativeLayout{ *window, x, y, width, height } { }
    };


    enum class AlignmentVertical { Top, Center, Bottom };
    enum class AlignmentHorizontal { Left, Middle, Right };

    using Alignment = std::pair<AlignmentHorizontal, AlignmentVertical>;


    [[nodiscard]] u32 get_horizontal_alignment_offset(const Layout& layout, AlignmentHorizontal alignment, u32 width);


    [[nodiscard]] u32 get_vertical_alignment_offset(const Layout& layout, AlignmentVertical alignment, u32 height);

    [[nodiscard]] Rect get_rectangle_aligned(const Layout& layout, u32 width, u32 height, const Alignment& alignment);
} // namespace ui
