
#include "ui/layout.hpp"


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


[[nodiscard]] shapes::Rect
ui::get_rectangle_aligned(const Layout& layout, const std::pair<u32, u32>& size, const Alignment& alignment) {

    const auto width = size.first;
    const auto height = size.second;

    const auto offset_x = get_horizontal_alignment_offset(layout, alignment.first, width);
    const auto offset_y = get_vertical_alignment_offset(layout, alignment.second, height);


    return shapes::Rect{ static_cast<int>(offset_x), static_cast<int>(offset_y), static_cast<int>(width),
                         static_cast<int>(height) };
}


[[nodiscard]] std::pair<u32, u32>
ui::ratio_helper(const std::pair<u32, u32>& size, bool respect_ratio, const shapes::Point& original_ratio) {

    if (not respect_ratio) {
        return size;
    }

    const double dest_ratio = static_cast<double>(original_ratio.x) / static_cast<double>(original_ratio.y);

    const double current_ratio = static_cast<double>(size.first) / static_cast<double>(size.second);

    if (current_ratio >= dest_ratio) {
        return { static_cast<u32>(dest_ratio * size.first / current_ratio), size.second };
    }

    return { size.first, static_cast<u32>(size.second * current_ratio / dest_ratio) };
}
