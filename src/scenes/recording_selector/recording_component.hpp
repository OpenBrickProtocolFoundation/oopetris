

#pragma once

#include "ui/components/label.hpp"
#include "ui/focusable.hpp"
#include "ui/hoverable.hpp"
#include "ui/layouts/tile_layout.hpp"
#include "ui/widget.hpp"

#include <filesystem>
#include <utility>

namespace data {

    enum class RecordingSource : u8 { Commandline, Folder, Manual, Online };

    struct RecordingMetadata {
        std::filesystem::path path;
        RecordingSource source;
    };

} // namespace data


namespace custom_ui {


    struct RecordingComponent final : public ui::Widget, public ui::Focusable, public ui::Hoverable {
    private:
        ui::TileLayout m_main_layout;
        data::RecordingMetadata m_metadata;

    public:
        explicit RecordingComponent(
                ServiceProvider* service_provider,
                ui::FocusHelper& focus_helper,
                const data::RecordingMetadata& metadata,
                const ui::Layout& layout,
                bool is_top_level
        );

        void render(const ServiceProvider& service_provider) const override;

        helper::BoolWrapper<ui::EventHandleType> handle_event(const SDL_Event& event, const Window* window) override;

    private:
        [[nodiscard]] std::tuple<ui::Label*, ui::Label*, ui::Label*, ui::Label*> get_texts();

        void populate_texts();
    };


} // namespace custom_ui
