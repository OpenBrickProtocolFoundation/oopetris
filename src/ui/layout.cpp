
#include "layout.hpp"


[[nodiscard]] u32 ui::get_horizontal_alignment_offset(const Layout& layout, AlignmentHorizontal alignment, u32 width) {
    switch (alignment) {
        case AlignmentHorizontal::Right:
            return layout.get_rect().top_left.x;
        case AlignmentHorizontal::Middle:
            return layout.get_rect().top_left.x + (layout.get_rect().width() / 2) - (width / 2);
        case AlignmentHorizontal::Left:
            return layout.get_rect().bottom_right.x - width;
        default:
            utils::unreachable();
    }
}

[[nodiscard]] u32 ui::get_vertical_alignment_offset(const Layout& layout, AlignmentVertical alignment, u32 height) {
    switch (alignment) {
        case AlignmentVertical::Top:
            return layout.get_rect().top_left.y;
        case AlignmentVertical::Center:
            return layout.get_rect().top_left.y + (layout.get_rect().height() / 2) - (height / 2);
        case AlignmentVertical::Bottom:
            return layout.get_rect().bottom_right.y - height;
        default:
            utils::unreachable();
    }
}


[[nodiscard]] Rect ui::get_rectangle_aligned(const Layout& layout, u32 width, u32 height, const Alignment& alignment) {

    const auto offset_x = get_horizontal_alignment_offset(layout, alignment.first, width);
    const auto offset_y = get_vertical_alignment_offset(layout, alignment.second, height);


    return Rect{ static_cast<int>(offset_x), static_cast<int>(offset_y), static_cast<int>(width),
                 static_cast<int>(height) };
}
