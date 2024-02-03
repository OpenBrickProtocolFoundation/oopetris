#include "settings_menu.hpp"
#include "../../constants.hpp"
#include "../../music_manager.hpp"
#include "../../resource_manager.hpp"
#include "../../window.hpp"

namespace scenes {

    SettingsMenu::SettingsMenu(ServiceProvider* service_provider) : Scene{ SceneId::SettingsMenu, service_provider }
, m_main_grid{
    ui::FullScreenLayout{ service_provider->window() },
    ui::Direction::Vertical,
    ui::RelativeMargin{ service_provider->window(), ui::Direction::Vertical, 0.05 },
    std::pair<double, double>{ 0.05, 0.05 } 
} {

        m_main_grid.add<ui::Label>(
                0, "Settings", Color::white(), service_provider->fonts().get(FontId::Default),
                std::pair<double, double>{ 0.8, 0.4 },
                ui::Alignment{ ui::AlignmentHorizontal::Middle, ui::AlignmentVertical::Center }
        );


        m_main_grid.add<ui::Slider>(
                1, "Volume", 100, ui::Slider::Range{ 0.0F, 1.0F },
                [service_provider]() {
                    const auto value = service_provider->music_manager().get_volume();
                    return value.has_value() ? value.value() : 0.0F;
                },
                [service_provider](const float& amount) {
                    const auto mapped_amount = amount <= 0.0F ? tl::nullopt : tl::make_optional(amount);
                    return service_provider->music_manager().set_volume(mapped_amount);
                },
                0.05F, std::pair<double, double>{ 0.8, 0.4 },
                ui::Alignment{ ui::AlignmentHorizontal::Middle, ui::AlignmentVertical::Center }
        );

        m_main_grid.add<ui::Button>(
                2, "Return", 100, [this](const ui::Button&) { m_should_exit = true; },
                std::pair<double, double>{ 0.8, 0.4 },
                ui::Alignment{ ui::AlignmentHorizontal::Middle, ui::AlignmentVertical::Center },
                std::pair<double, double>{ 0, 0 }
        );
    }

    [[nodiscard]] Scene::UpdateResult SettingsMenu::update() {
        if (m_should_exit) {
            return UpdateResult{ SceneUpdate::StopUpdating, Scene::Pop{} };
        }
        return UpdateResult{ SceneUpdate::StopUpdating, tl::nullopt };
    }

    void SettingsMenu::render(const ServiceProvider& service_provider) {

        const auto is_above_ingame =
                m_service_provider->active_scenes().size() >= 2
                && m_service_provider->active_scenes().at(m_service_provider->active_scenes().size() - 2)->get_id()
                           == SceneId::Ingame;

        if (is_above_ingame) {
            //TODO: make relative and as layout !!
            const auto whole_settings_rect = Rect{
                Point{ 90,  90},
                Point{510, 350}
            };
            service_provider.renderer().draw_rect_filled(whole_settings_rect, Color::black());
            service_provider.renderer().draw_rect_outline(whole_settings_rect, Color::white());
        } else {
            service_provider.renderer().draw_rect_filled(service_provider.window().screen_rect(), Color::black());
        }

        m_main_grid.render(service_provider);
    }

    bool SettingsMenu::handle_event(const SDL_Event& event) {
        if (m_main_grid.handle_event(event)) {
            return true;
        }

        if (utils::event_is_action(event, utils::CrossPlatformAction::CLOSE)) {
            m_should_exit = true;
            return true;
        }

        if (utils::event_is_action(event, utils::CrossPlatformAction::OPEN_SETTINGS)) {
            m_should_exit = true;
            return true;
        }

        // return true all the time, since no scene then the settings should receive inputs (since we draw black over them anyway!)
        return true;
    }

} // namespace scenes
