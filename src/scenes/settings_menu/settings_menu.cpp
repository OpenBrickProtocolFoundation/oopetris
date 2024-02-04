#include "settings_menu.hpp"
#include "../../constants.hpp"
#include "../../music_manager.hpp"
#include "../../resource_manager.hpp"
#include "../../window.hpp"

namespace scenes {

    SettingsMenu::SettingsMenu(ServiceProvider* service_provider, const  ui::Layout& layout) : Scene{service_provider, layout}
, m_main_grid{
    ui::RelativeLayout{layout.get_rect(), 0.0, 0.2, 1.0, 0.5  },
    ui::Direction::Vertical,
    ui::RelativeMargin{ layout.get_rect(), ui::Direction::Vertical, 0.05 },
    std::pair<double, double>{ 0.05, 0.05 } 
} {

        auto id_helper = ui::IDHelper{};

        m_main_grid.add<ui::Label>(
                id_helper.index(), "Settings", Color::white(), service_provider->fonts().get(FontId::Default),
                std::pair<double, double>{ 0.3, 0.6 },
                ui::Alignment{ ui::AlignmentHorizontal::Middle, ui::AlignmentVertical::Center }
        );


        m_main_grid.add<ui::Label>(
                id_helper.index(), "Volume", Color::white(), service_provider->fonts().get(FontId::Default),
                std::pair<double, double>{ 0.1, 0.3 },
                ui::Alignment{ ui::AlignmentHorizontal::Middle, ui::AlignmentVertical::Bottom }
        );

        const auto slider_index = id_helper.index();
        m_main_grid.add<ui::Slider>(
                slider_index, id_helper.focus_id(), ui::Slider::Range{ 0.0F, 1.0F },
                [service_provider]() {
                    const auto value = service_provider->music_manager().get_volume();
                    return value.has_value() ? value.value() : 0.0F;
                },
                [service_provider](const float& amount) {
                    const auto mapped_amount = amount <= 0.0F ? tl::nullopt : tl::make_optional(amount);
                    return service_provider->music_manager().set_volume(mapped_amount, false, false);
                },
                0.05F, std::pair<double, double>{ 0.6, 1.0 },
                ui::Alignment{ ui::AlignmentHorizontal::Middle, ui::AlignmentVertical::Center }
        );

        service_provider->music_manager().add_volume_listener(listener_name, [this, slider_index](tl::optional<float>) {
            this->m_main_grid.get<ui::Slider>(slider_index)->on_change();
        });

        m_main_grid.add<ui::Button>(
                id_helper.index(), "Return", id_helper.focus_id(), [this](const ui::Button&) { m_should_exit = true; },
                std::pair<double, double>{ 0.15, 0.85 },
                ui::Alignment{ ui::AlignmentHorizontal::Middle, ui::AlignmentVertical::Center },
                std::pair<double, double>{ 0.1, 0.1 }
        );
    }

    [[nodiscard]] Scene::UpdateResult SettingsMenu::update() {
        if (m_should_exit) {
            m_service_provider->music_manager().remove_volume_listener(listener_name);
            return UpdateResult{ SceneUpdate::StopUpdating, Scene::Pop{} };
        }
        return UpdateResult{ SceneUpdate::StopUpdating, tl::nullopt };
    }

    void SettingsMenu::render(const ServiceProvider& service_provider) {

        const auto layout = get_layout();
        service_provider.renderer().draw_rect_filled(layout.get_rect(), Color::black());

        if (not layout.is_full_screen()) {
            service_provider.renderer().draw_rect_outline(layout.get_rect(), Color::white());
        }

        m_main_grid.render(service_provider);
    }

    bool SettingsMenu::handle_event(const SDL_Event& event, const Window* window) {
        if (m_main_grid.handle_event(event, window)) {
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

        return false;
    }

} // namespace scenes
