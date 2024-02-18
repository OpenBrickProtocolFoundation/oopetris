#pragma once

#include "scenes/scene.hpp"
#include "ui/button.hpp"
#include "ui/label.hpp"
#include "ui/tile_layout.hpp"

namespace scenes {

    struct OnlineLobby : public Scene {
    private:
        enum class Command { Play, Return };

        ui::TileLayout<3> m_main_layout;
        helpers::optional<Command> m_next_command;

    public:
        explicit OnlineLobby(ServiceProvider* service_provider, const ui::Layout& layout);

        [[nodiscard]] UpdateResult update() override;
        void render(const ServiceProvider& service_provider) override;
        bool handle_event(const SDL_Event& event, const Window* window) override;
    };

} // namespace scenes
