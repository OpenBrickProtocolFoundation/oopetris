

#pragma once

#include <core/helper/color_literals.hpp>
#include <recordings/recordings.hpp>

#include "ui/components/label.hpp"
#include "ui/focusable.hpp"
#include "ui/hoverable.hpp"
#include "ui/layouts/tile_layout.hpp"
#include "ui/widget.hpp"

#include <filesystem>
#include <utility>

namespace data {

    //TODO(Totto):  add drop support
    enum class RecordingSource : u8 { CommandLine, Folder, Manual, Online, Drop };

    struct RecordingMetadata {
        std::filesystem::path path;
        RecordingSource source;
        std::vector<recorder::TetrionHeader> headers;
        recorder::AdditionalInformation information;
    };

} // namespace data


namespace custom_ui {

    struct RecordingComponent final : public ui::Widget, public ui::Focusable, public ui::Hoverable {
    private:
        ui::TileLayout m_main_layout;
        data::RecordingMetadata m_metadata;
        u32 m_current_focus_id;

    public:
        OOPETRIS_GRAPHICS_EXPORTED explicit RecordingComponent(
                ServiceProvider* service_provider,
                ui::FocusHelper& focus_helper,
                data::RecordingMetadata metadata,
                const ui::Layout& layout,
                bool is_top_level
        );

        OOPETRIS_GRAPHICS_EXPORTED void render(const ServiceProvider& service_provider) const override;

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] Widget::EventHandleResult
        handle_event(const std::shared_ptr<input::InputManager>& input_manager, const SDL_Event& event) override;

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] data::RecordingMetadata metadata() const;

    private:
        [[nodiscard]] std::tuple<ui::Label*, ui::Label*, ui::Label*, ui::Label*> get_texts();

        void populate_texts(ServiceProvider* service_provider);
    };


} // namespace custom_ui
