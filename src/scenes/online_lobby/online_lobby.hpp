#pragma once

#include "lobby/api.hpp"

#include "scenes/scene.hpp"
#include "ui/components/label.hpp"
#include "ui/components/text_button.hpp"
#include "ui/layouts/tile_layout.hpp"

#include <memory>

namespace scenes {

    struct OnlineLobby : public Scene {
    private:
        enum class Command : u8 { Play, Return };

        ui::TileLayout m_main_layout;
        std::optional<Command> m_next_command;
        std::unique_ptr<lobby::API> m_api;

    public:
        OOPETRIS_GRAPHICS_EXPORTED explicit OnlineLobby(ServiceProvider* service_provider, const ui::Layout& layout);

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] UpdateResult update() override;
        OOPETRIS_GRAPHICS_EXPORTED void render(const ServiceProvider& service_provider) override;
        OOPETRIS_GRAPHICS_EXPORTED bool
        handle_event(const std::shared_ptr<input::InputManager>& input_manager, const SDL_Event& event) override;
    };

} // namespace scenes
