#pragma once

#include "game/game.hpp"
#include "scenes/scene.hpp"

namespace scenes {

    struct ReplayGame : public Scene {
    private:
        enum class NextScene : u8 { Pause, Settings };


        std::optional<NextScene> m_next_scene{};
        std::vector<std::unique_ptr<Game>> m_games;

    public:
        explicit ReplayGame(
                ServiceProvider* service_provider,
                const ui::Layout& layout,
                const std::filesystem::path& recording_path
        );

        [[nodiscard]] UpdateResult update() override;
        void render(const ServiceProvider& service_provider) override;
        [[nodiscard]] bool
        handle_event(const std::shared_ptr<input::InputManager>& input_manager, const SDL_Event& event) override;
    };


} // namespace scenes
