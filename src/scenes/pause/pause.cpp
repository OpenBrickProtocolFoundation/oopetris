#include "pause.hpp"
#include "../../capabilities.hpp"
#include "../../renderer.hpp"
#include "../../resource_manager.hpp"
#include <fmt/format.h>


#if defined(__SWITCH__)
#include "../../switch_buttons.hpp"
#endif
namespace scenes {

    Pause::Pause(ServiceProvider* service_provider) : Scene{ SceneId::Pause, service_provider }, m_heading {
        fmt::format(
            "Pause ({}: continue, {}: quit)",
            utils::action_description(utils::CrossPlatformAction::UNPAUSE),
            utils::action_description(utils::CrossPlatformAction::EXIT)
        ),
        Color::white(),
        service_provider->fonts().get(FontId::Default),
        std::pair<double, double>{ 0.8, 0.4 },
                ui::Alignment{ ui::AlignmentHorizontal::Middle, ui::AlignmentVertical::Center },
                ui::FullScreenLayout{service_provider->window()}
    }
    { }

    [[nodiscard]] Scene::UpdateResult scenes::Pause::update() {
        if (m_should_unpause) {
            return UpdateResult{ SceneUpdate::StopUpdating, Scene::Pop{} };
        }
        if (m_should_exit) {
            return UpdateResult{ SceneUpdate::StopUpdating, Scene::Switch{ SceneId::MainMenu } };
        }
        return UpdateResult{ SceneUpdate::StopUpdating, tl::nullopt };
    }

    void Pause::render(const ServiceProvider& service_provider) {
        service_provider.renderer().draw_rect_filled(service_provider.window().screen_rect(), Color::black(180));
        m_heading.render(service_provider);
    }

    [[nodiscard]] bool Pause::handle_event(const SDL_Event& event, const Window*) {

        if (utils::event_is_action(event, utils::CrossPlatformAction::UNPAUSE)) {
            m_should_unpause = true;
            return true;
        }

        if (utils::event_is_action(event, utils::CrossPlatformAction::EXIT)) {
            m_should_exit = true;
            return true;
        }

        return false;
    }

} // namespace scenes
