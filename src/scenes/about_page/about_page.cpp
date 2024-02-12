#include "about_page.hpp"
#include "graphics/window.hpp"
#include "helper/constants.hpp"
#include "helper/git_helper.hpp"
#include "helper/utils.hpp"
#include "manager/resource_manager.hpp"
#include "platform/capabilities.hpp"
#include "ui/image_view.hpp"
#include "ui/link_label.hpp"
#include "ui/tile_layout.hpp"

#include <fmt/format.h>

namespace scenes {

    AboutPage::AboutPage(ServiceProvider* service_provider, const  ui::Layout& layout) : Scene{service_provider, layout}
, m_main_grid{
    ui::Direction::Vertical,
    ui::RelativeMargin{ layout, ui::Direction::Vertical, 0.05 },
    std::pair<double, double>{ 0.05, 0.05 } ,
    ui::RelativeLayout{layout, 0.0, 0.2, 1.0, 0.5  }
} {

        auto id_helper = ui::IDHelper{};

        m_main_grid.add<ui::Label>(
                id_helper.index(), fmt::format("Git Commit: {}", utils::git_commit()), Color::white(),
                service_provider->fonts().get(FontId::Default), std::pair<double, double>{ 0.3, 0.6 },
                ui::Alignment{ ui::AlignmentHorizontal::Middle, ui::AlignmentVertical::Center }
        );


        m_main_grid.add<ui::Label>(
                id_helper.index(), fmt::format("Build for: {}", utils::built_for_platform()), Color::white(),
                service_provider->fonts().get(FontId::Default), std::pair<double, double>{ 0.3, 0.6 },
                ui::Alignment{ ui::AlignmentHorizontal::Middle, ui::AlignmentVertical::Center }
        );


        m_main_grid.add<ui::Label>(
                id_helper.index(), "Authors:", Color::white(), service_provider->fonts().get(FontId::Default),
                std::pair<double, double>{ 0.3, 0.6 },
                ui::Alignment{ ui::AlignmentHorizontal::Middle, ui::AlignmentVertical::Center }
        );

        const std::vector<std::tuple<std::string, std::string, std::string>> authors = {
            {"mgerhold", "https://github.com/mgerhold", "mgerhold.jpg"},
            { "Totto16",  "https://github.com/Totto16",  "Totto16.png"}
        };

        for (const auto& [name, link, image_name] : authors) {


            const auto index = id_helper.index();

            if (index >= m_main_grid.size()) {
                throw std::runtime_error("To much authors or to less space in the grid layout!");
            }

            m_main_grid.add<ui::TileLayout<2>>(
                    index, ui::Direction::Horizontal, std::array<double, 1>{ 0.85 }, ui::AbsolutMargin{ 0 },
                    std::pair<double, double>{ 0.05, 0.03 }
            );

            ui::TileLayout<2>* tile_layout = m_main_grid.get<ui::TileLayout<2>>(index);

            auto local_id_helper = ui::IDHelper{};


            tile_layout->add<ui::LinkLabel>(
                    local_id_helper.index(), name, link, Color::white(), Color{ 0xA1, 0x9F, 0x9F },
                    service_provider->fonts().get(FontId::Default), std::pair<double, double>{ 0.9, 0.8 },
                    ui::Alignment{ ui::AlignmentHorizontal::Middle, ui::AlignmentVertical::Center }
            );

            const auto final_path = utils::get_assets_folder() / "authors" / image_name;

            tile_layout->add<ui::ImageView>(
                    local_id_helper.index(), final_path, std::pair<double, double>{ 0.9, 0.8 },
                    ui::Alignment{ ui::AlignmentHorizontal::Middle, ui::AlignmentVertical::Center }
            );
        }
    }

    [[nodiscard]] Scene::UpdateResult AboutPage::update() {
        if (m_should_exit) {
            return UpdateResult{ SceneUpdate::StopUpdating, Scene::Pop{} };
        }
        return UpdateResult{ SceneUpdate::StopUpdating, tl::nullopt };
    }

    void AboutPage::render(const ServiceProvider& service_provider) {

        const auto layout = get_layout();
        service_provider.renderer().draw_rect_filled(layout.get_rect(), Color::black());

        m_main_grid.render(service_provider);
    }

    bool AboutPage::handle_event(const SDL_Event& event, const Window* window) {
        if (m_main_grid.handle_event(event, window)) {
            return true;
        }

        if (utils::event_is_action(event, utils::CrossPlatformAction::CLOSE)) {
            m_should_exit = true;
            return true;
        }

        return false;
    }

} // namespace scenes
