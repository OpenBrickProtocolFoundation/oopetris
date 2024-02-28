#pragma once

#include "game/game.hpp"
#include "helper/command_line_arguments.hpp"
#include "manager/recording.hpp"
#include "scenes/scene.hpp"

namespace scenes {

    struct SinglePlayerGame : public Scene {
    private:
        enum class NextScene : u8 { Pause, Settings };


        helper::optional<NextScene> m_next_scene{};
        std::unique_ptr<Game> m_game;

    public:
        explicit SinglePlayerGame(ServiceProvider* service_provider, const ui::Layout& layout);

        [[nodiscard]] UpdateResult update() override;
        void render(const ServiceProvider& service_provider) override;
        [[nodiscard]] bool handle_event(const SDL_Event& event, const Window* window) override;
    };


} // namespace scenes
