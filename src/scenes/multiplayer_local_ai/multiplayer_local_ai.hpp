#pragma once

#include "game/game.hpp"
#include "game/tetrion.hpp"
#include "scenes/scene.hpp"

#include "plugins/ai/ai.hpp"

namespace scenes {

    struct MultiPlayerLocalAIGame : public Scene {
    private:
        enum class NextScene : u8 { Pause, Settings };

        std::optional<NextScene> m_next_scene;
        std::unique_ptr<Game> m_my_game;
        std::unique_ptr<Game> m_ai_game;
        std::unique_ptr<oopetris::AI> m_ai;


    public:
        OOPETRIS_GRAPHICS_EXPORTED explicit MultiPlayerLocalAIGame(
                ServiceProvider* service_provider,
                const ui::Layout& layout,
                std::unique_ptr<oopetris::AI>&& input_ai
        );

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] UpdateResult update() override;
        OOPETRIS_GRAPHICS_EXPORTED void render(const ServiceProvider& service_provider) override;
        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] bool
        handle_event(const std::shared_ptr<input::InputManager>& input_manager, const SDL_Event& event) override;
    };


} // namespace scenes
