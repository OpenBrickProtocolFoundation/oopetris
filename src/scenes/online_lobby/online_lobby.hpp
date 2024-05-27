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
        std::unique_ptr<lobby::Client> m_client{ nullptr };

    public:
        explicit OnlineLobby(ServiceProvider* service_provider, const ui::Layout& layout);

        [[nodiscard]] UpdateResult update() override;
        void render(const ServiceProvider& service_provider) override;
        bool handle_event(const std::shared_ptr<input::InputManager>& input_manager, const SDL_Event& event) override;
    };

} // namespace scenes
