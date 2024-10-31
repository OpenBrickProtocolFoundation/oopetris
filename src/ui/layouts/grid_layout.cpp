
#include "grid_layout.hpp"


ui::GridLayout::GridLayout(
                u32 focus_id, // NOLINT(bugprone-easily-swappable-parameters)
                u32 size,
                Direction direction,
                Margin gap,
                std::pair<double, double> margin,
                const Layout& layout,
                bool is_top_level 
        )
            : FocusLayout{ layout, focus_id, FocusOptions{ .wrap_around=is_top_level, .allow_tab=is_top_level }, is_top_level },
              m_size{ size },
              m_direction{ direction },
              m_gap{ gap },
              m_margin{ static_cast<u32>(margin.first * layout.get_rect().width()),
                      static_cast<u32>(margin.second * layout.get_rect().height()) } {
        }

[[nodiscard]] u32 ui::GridLayout::total_size() const {
    return m_size;
}

void ui::GridLayout::render(const ServiceProvider& service_provider) const {
    for (const auto& widget : m_widgets) {
        widget->render(service_provider);
    }
}


[[nodiscard]] ui::Layout ui::GridLayout::get_layout_for_index(u32 index) {
    if (index >= this->m_size) {
        throw std::runtime_error("GridLayout is already full");
    }

    const auto start_point = layout().get_rect().top_left;

    u32 x_pos = start_point.x + m_margin.first;
    u32 y_pos = start_point.y + m_margin.second;
    u32 width = layout().get_rect().width() - (m_margin.first * 2);
    u32 height = layout().get_rect().height() - (m_margin.second * 2);

    if (m_direction == Direction::Horizontal) {
        const u32 total_margin = this->m_size <= 1 ? 0 : (this->m_size - 1) * m_gap.get_margin();
        assert(layout().get_rect().width() > (total_margin + (m_margin.first * 2))
               && "width has to be greater than the margins");
        width = (layout().get_rect().width() - total_margin - (m_margin.first * 2)) / this->m_size;

        const u32 margin_x = index * m_gap.get_margin();
        const u32 total_width = width * index;
        x_pos += margin_x + total_width;
    } else {
        const u32 total_margin = this->m_size <= 1 ? 0 : (this->m_size - 1) * m_gap.get_margin();
        assert(layout().get_rect().height() > (total_margin - (m_margin.second * 2))
               && "height has to be greater than the margins");
        height = (layout().get_rect().height() - total_margin - (m_margin.second * 2)) / this->m_size;

        const u32 margin_y = index * m_gap.get_margin();
        const u32 total_height = height * index;
        y_pos += margin_y + total_height;
    }


    return AbsolutLayout{
        x_pos,
        y_pos,
        width,
        height,
    };
}
