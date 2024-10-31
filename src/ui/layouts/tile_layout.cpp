
#include "tile_layout.hpp"


void ui::TileLayout::render(const ServiceProvider& service_provider) const {
    for (const auto& widget : m_widgets) {
        widget->render(service_provider);
    }
}


[[nodiscard]] ui::Layout ui::TileLayout::get_layout_for_index(u32 index) {
    if (index >= this->size) {
        throw std::runtime_error("TileLayout is already full");
    }

    const auto start_point = layout().get_rect().top_left;

    u32 x_pos = start_point.x + margin.first;
    u32 y_pos = start_point.y + margin.second;
    u32 width = layout().get_rect().width() - (margin.first * 2);
    u32 height = layout().get_rect().height() - (margin.second * 2);

    if (direction == Direction::Horizontal) {
        const auto previous_start =
                index == 0 ? 0 : static_cast<u32>(width * steps.at(index - 1)) + (gap.get_margin() / 2);

        const auto current_end =
                index == this->size - 1
                        ? width
                        : (steps.size() <= index ? width
                                                 : static_cast<u32>(width * steps.at(index)) - (gap.get_margin() / 2));

        width = current_end - previous_start;
        x_pos += previous_start;
    } else {
        const auto previous_start =
                index == 0 ? 0 : static_cast<u32>(height * steps.at(index - 1)) + (gap.get_margin() / 2);

        const auto current_end =
                index == this->size - 1
                        ? height
                        : (steps.size() <= index ? height
                                                 : static_cast<u32>(height * steps.at(index)) - (gap.get_margin() / 2));

        height = current_end - previous_start;
        y_pos += previous_start;
    }


    return AbsolutLayout{
        x_pos,
        y_pos,
        width,
        height,
    };
}
