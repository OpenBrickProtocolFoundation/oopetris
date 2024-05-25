#pragma once

#include "scenes/scene.hpp"
#include "ui/layouts/tile_layout.hpp"
#include "ui/widget.hpp"

#include <filesystem>
#include <variant>

namespace details::recording::selector {

    struct Return { };

    struct Action {
        ui::Widget* widget;
    };

    struct Command {
        using Value = std::variant<Return, Action>;
        Value m_value;

        template<typename T>
        explicit Command(T value) : m_value{ std::move(value) } {
            //
        }
    };


} // namespace details::recording::selector

namespace scenes {

    struct RecordingSelector : public Scene {
    private:
        ui::TileLayout m_main_layout;
        std::optional<details::recording::selector::Command> m_next_command{ std::nullopt };
        std::vector<std::filesystem::path> m_chosen_paths{};

    public:
        explicit RecordingSelector(ServiceProvider* service_provider, const ui::Layout& layout);

        [[nodiscard]] UpdateResult update() override;
        void render(const ServiceProvider& service_provider) override;
        bool handle_event(const std::shared_ptr<input::InputManager>& input_manager, const SDL_Event& event) override;

    private:
        void add_all_recordings();
    };

} // namespace scenes
