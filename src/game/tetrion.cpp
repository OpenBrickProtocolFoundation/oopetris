#include <core/helper/magic_enum_wrapper.hpp>
#include <core/helper/utils.hpp>


#include "game/simulated_tetrion.hpp"
#include "helper/platform.hpp"
#include "manager/resource_manager.hpp"
#include "tetrion.hpp"
#include "ui/components/label.hpp"

#include <spdlog/spdlog.h>
#include <sstream>


Tetrion::Tetrion(
        const u8 tetrion_index,
        const Random::Seed random_seed,
        const u32 starting_level,
        ServiceProvider* const service_provider,
        std::optional<std::shared_ptr<recorder::RecordingWriter>> recording_writer,
        const ui::Layout& layout,
        bool is_top_level
)
    : ui::Widget{ layout , ui::WidgetType::Component ,is_top_level},
        SimulatedTetrion{tetrion_index,random_seed,starting_level, service_provider,std::move(recording_writer)},
      m_main_layout{
                utils::SizeIdentity<2>(),
                0,
                ui::Direction::Vertical,
                { 0.85 },
                ui::AbsolutMargin{ 0 },
                std::pair<double, double>{ 0.05, 0.03 },
                layout
       } {

    m_main_layout.add<Grid>();

    m_main_layout.add<ui::GridLayout>(
            1, 3, ui::Direction::Vertical, ui::AbsolutMargin{ 0 }, std::pair<double, double>{ 0.0, 0.1 }
    );

    auto* text_layout = get_text_layout();

    constexpr auto text_size = utils::get_orientation() == utils::Orientation::Landscape
                                       ? std::pair<double, double>{ 0.2, 0.8 }
                                       : std::pair<double, double>{ 0.6, 0.8 };

    text_layout->add<ui::Label>(
            service_provider, "score: 0", service_provider->font_manager().get(FontId::Default), Color::white(),
            text_size, ui::Alignment{ ui::AlignmentHorizontal::Middle, ui::AlignmentVertical::Center }
    );


    text_layout->add<ui::Label>(
            service_provider, "lines: 0", service_provider->font_manager().get(FontId::Default), Color::white(),
            text_size, ui::Alignment{ ui::AlignmentHorizontal::Middle, ui::AlignmentVertical::Center }
    );

    text_layout->add<ui::Label>(
            service_provider, "lines: 0", service_provider->font_manager().get(FontId::Default), Color::white(),
            text_size, ui::Alignment{ ui::AlignmentHorizontal::Middle, ui::AlignmentVertical::Center }
    );

    refresh_texts();
}

Tetrion::~Tetrion() = default;

void Tetrion::render(const ServiceProvider& service_provider) const {

    m_main_layout.render(service_provider);

    const auto* grid = get_grid();
    const double original_scale = grid->scale_to_original();
    const ScreenCordsFunction to_screen_coords = [grid](const grid::GridPoint& point) {
        return grid->to_screen_coords(point);
    };
    const shapes::UPoint& tile_size = grid->tile_size();

    helper::graphics::render_minos(m_mino_stack, service_provider, original_scale, to_screen_coords, tile_size);
    if (m_active_tetromino.has_value()) {
        m_active_tetromino->render(
                service_provider, MinoTransparency::Solid, original_scale, to_screen_coords, tile_size,
                grid::grid_position
        );
    }
    if (m_ghost_tetromino.has_value()) {
        m_ghost_tetromino->render(
                service_provider, MinoTransparency::Ghost, original_scale, to_screen_coords, tile_size,
                grid::grid_position
        );
    }
    for (std::underlying_type_t<MinoTransparency> i = 0; i < static_cast<decltype(i)>(m_preview_tetrominos.size());
         ++i) {
        if (const auto current_preview_tetromino = m_preview_tetrominos.at(i); current_preview_tetromino.has_value()) {
            static constexpr auto enum_index = magic_enum::enum_index(MinoTransparency::Preview0);
            static_assert(enum_index.has_value());
            const auto transparency = magic_enum::enum_value<MinoTransparency>(
                    enum_index.value() + i // NOLINT(bugprone-unchecked-optional-access)
            );
            current_preview_tetromino->render(
                    service_provider, transparency, original_scale, to_screen_coords, tile_size
            );
        }
    }
    if (m_tetromino_on_hold) {
        m_tetromino_on_hold->render(
                service_provider, MinoTransparency::Solid, original_scale, to_screen_coords, tile_size
        );
    }
}

[[nodiscard]] ui::Widget::EventHandleResult
Tetrion::handle_event(const std::shared_ptr<input::InputManager>& /*input_manager*/, const SDL_Event& /*event*/) {
    return false;
}

[[nodiscard]] Grid* Tetrion::get_grid() {
    return m_main_layout.get<Grid>(0);
}

[[nodiscard]] const Grid* Tetrion::get_grid() const {
    return m_main_layout.get<Grid>(0);
}

[[nodiscard]] ui::GridLayout* Tetrion::get_text_layout() {
    return m_main_layout.get<ui::GridLayout>(1);
}

[[nodiscard]] const ui::GridLayout* Tetrion::get_text_layout() const {
    return m_main_layout.get<ui::GridLayout>(1);
}

void Tetrion::refresh_texts() {
    auto* text_layout = get_text_layout();

    std::stringstream stream;
    stream << "score: " << m_score;
    text_layout->get<ui::Label>(0)->set_text(*m_service_provider, stream.str());

    stream = std::stringstream{};
    stream << "level: " << m_level;
    text_layout->get<ui::Label>(1)->set_text(*m_service_provider, stream.str());

    stream = std::stringstream{};
    stream << "lines: " << m_lines_cleared;
    text_layout->get<ui::Label>(2)->set_text(*m_service_provider, stream.str());
}
