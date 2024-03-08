#pragma once

#include "lobby/api.hpp"
#include "scenes/scene.hpp"
#include "ui/components/button.hpp"
#include "ui/components/label.hpp"
#include "ui/layouts/tile_layout.hpp"

#include <filesystem>
#include <memory>

namespace data {

    enum class RecordingSource : u8 { Commandline, Folder, Manual, Online };

    struct RecordingMetadata {
        std::filesystem::path path;
        RecordingSource source;
    };

} // namespace data


namespace scenes {

    struct RecordingSelector : public Scene {
    private:
        enum class Command : u8 { Play, Return };

        ui::TileLayout m_main_layout;
        helper::optional<Command> m_next_command;
        std::unique_ptr<lobby::Client> client{ nullptr };

    public:
        explicit RecordingSelector(ServiceProvider* service_provider, const ui::Layout& layout);

        [[nodiscard]] UpdateResult update() override;
        void render(const ServiceProvider& service_provider) override;
        bool handle_event(const SDL_Event& event, const Window* window) override;

    private:
        void add_all_recordings();
    };

} // namespace scenes
