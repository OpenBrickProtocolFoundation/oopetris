#include "ai_game.hpp"
#include "external/load_file.hpp"
#include "graphics/window.hpp"
#include "helper/constants.hpp"
#include "helper/git_helper.hpp"
#include "helper/utils.hpp"
#include "manager/resource_manager.hpp"

#include <fmt/format.h>

namespace scenes {

    AIGame::AIGame(ServiceProvider* service_provider, const ui::Layout& layout) : Scene{ service_provider, layout } , m_main_grid{3,
    ui::Direction::Vertical,
    ui::RelativeMargin{ layout, ui::Direction::Vertical, 0.05 },
    std::pair<double, double>{ 0.05, 0.05 } ,
    ui::RelativeLayout{layout, 0.0, 0.2, 1.0, 0.5  }
} {

        auto id_helper = ui::IDHelper{};

        const auto run = []() {
            std::ignore = external::load_file(
                    std::filesystem::path("/home/totto/Code/coder2k/oopetris_pr5/src/external/examples/ai.tl")
            );
        };

        m_main_grid.add<ui::Label>(
                service_provider, "TEST SCENE", service_provider->fonts().get(FontId::Default), Color::white(),
                std::pair<double, double>{ 0.3, 0.6 },
                ui::Alignment{ ui::AlignmentHorizontal::Middle, ui::AlignmentVertical::Center }
        );

        m_main_grid.add<ui::Button>(
                service_provider, "rerun", service_provider->fonts().get(FontId::Default), Color::white(),
                id_helper.focus_id(), [this, &run](const ui::Button&) { run(); },
                std::pair<double, double>{ 0.15, 0.85 },
                ui::Alignment{ ui::AlignmentHorizontal::Middle, ui::AlignmentVertical::Center },
                std::pair<double, double>{ 0.1, 0.1 }
        );

        m_main_grid.add<ui::Button>(
                service_provider, "Return", service_provider->fonts().get(FontId::Default), Color::white(),
                id_helper.focus_id(), [this](const ui::Button&) { m_should_exit = true; },
                std::pair<double, double>{ 0.15, 0.85 },
                ui::Alignment{ ui::AlignmentHorizontal::Middle, ui::AlignmentVertical::Center },
                std::pair<double, double>{ 0.1, 0.1 }
        );

        run();
    }

    [[nodiscard]] Scene::UpdateResult AIGame::update() {
        m_main_grid.update();

        if (m_should_exit) {
            return UpdateResult{ SceneUpdate::StopUpdating, Scene::Pop{} };
        }
        return UpdateResult{ SceneUpdate::StopUpdating, helper::nullopt };
    }

    void AIGame::render(const ServiceProvider& service_provider) {
        const auto layout = get_layout();
        service_provider.renderer().draw_rect_filled(layout.get_rect(), Color::black());

        m_main_grid.render(service_provider);
    }

    bool AIGame::handle_event(const SDL_Event& event, const Window* window) {
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
