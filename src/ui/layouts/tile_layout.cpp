
#include "tile_layout.hpp"


void ui::TileLayout::render(const ServiceProvider& service_provider) const {
    for (const auto& widget : m_widgets) {
        widget->render(service_provider);
    }
}


[[nodiscard]] ui::Layout ui::TileLayout::get_layout_for_index(u32 index) {
    if (index >= this->m_size) {
        throw std::runtime_error("TileLayout is already full");
    }

    const auto start_point = layout().get_rect().top_left;

    u32 x_pos = start_point.x + m_margin.first;
    u32 y_pos = start_point.y + m_margin.second;
    u32 width = layout().get_rect().width() - (m_margin.first * 2);
    u32 height = layout().get_rect().height() - (m_margin.second * 2);

    if (m_direction == Direction::Horizontal) {
        const auto previous_start =
                index == 0 ? 0 : static_cast<u32>(width * m_steps.at(index - 1)) + (m_gap.get_margin() / 2);

        const auto current_end = index == this->m_size - 1
                                         ? width
                                         : (m_steps.size() <= index ? width
                                                                    : static_cast<u32>(width * m_steps.at(index))
                                                                              - (m_gap.get_margin() / 2));

        width = current_end - previous_start;
        x_pos += previous_start;
    } else {
        const auto previous_start =
                index == 0 ? 0 : static_cast<u32>(height * m_steps.at(index - 1)) + (m_gap.get_margin() / 2);

        const auto current_end = index == this->m_size - 1
                                         ? height
                                         : (m_steps.size() <= index ? height
                                                                    : static_cast<u32>(height * m_steps.at(index))
                                                                              - (m_gap.get_margin() / 2));

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
