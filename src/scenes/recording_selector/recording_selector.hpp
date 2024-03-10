#pragma once

#include "recording_component.hpp"
#include "scenes/scene.hpp"
#include "ui/components/button.hpp"
#include "ui/components/label.hpp"
#include "ui/layouts/tile_layout.hpp"

#include <memory>

namespace scenes {

    struct RecordingSelector : public Scene {
    private:
        enum class Command : u8 { Play, Return };

        ui::TileLayout m_main_layout;
        helper::optional<Command> m_next_command;

    public:
        explicit RecordingSelector(ServiceProvider* service_provider, const ui::Layout& layout);

        [[nodiscard]] UpdateResult update() override;
        void render(const ServiceProvider& service_provider) override;
        bool handle_event(const SDL_Event& event, const Window* window) override;

    private:
        void add_all_recordings();
    };

} // namespace scenes
