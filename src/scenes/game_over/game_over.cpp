#include "game_over.hpp"
#include "../../music_manager.hpp"
#include "../../renderer.hpp"
#include "../../resource_manager.hpp"

namespace scenes {

    GameOver::GameOver(ServiceProvider* service_provider) : Scene{ service_provider } {
        service_provider->music_manager()
                .load_and_play_music(
                        utils::get_assets_folder() / "music" / utils::get_supported_music_extension("05. Results")
                )
                .and_then(utils::log_error);
    }

    [[nodiscard]] Scene::UpdateResult GameOver::update() {
        if (m_should_exit) {
            return std::pair{ SceneUpdate::ContinueUpdating, Scene::Switch{ SceneId::MainMenu } };
        }
        return std::pair{ SceneUpdate::ContinueUpdating, tl::nullopt };
    }

    void GameOver::render(const ServiceProvider& service_provider) {
        service_provider.renderer().draw_rect_filled(service_provider.window().screen_rect(), Color::black(180));
        service_provider.renderer().draw_text(
                Point{ 100, 100 }, "Game Over, Press Return to continue", service_provider.fonts().get(FontId::Default),
                Color::white()
        );
    }

    bool GameOver::handle_event(const SDL_Event& event) {
        if (event.type == SDL_KEYDOWN and event.key.keysym.sym == SDLK_RETURN) {
            m_should_exit = true;
            return true;
        }
        return false;
    }
} // namespace scenes
