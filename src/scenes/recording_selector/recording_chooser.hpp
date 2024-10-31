

#pragma once

#include <core/helper/color_literals.hpp>

#include "ui/components/label.hpp"
#include "ui/focusable.hpp"
#include "ui/hoverable.hpp"
#include "ui/layouts/grid_layout.hpp"
#include "ui/widget.hpp"

namespace custom_ui {


    struct RecordingFileChooser final : public ui::Widget, public ui::Focusable, public ui::Hoverable {
    private:
        ui::GridLayout m_main_grid;
        std::vector<std::filesystem::path> m_currently_chosen_files;

    public:
        OOPETRIS_GRAPHICS_EXPORTED explicit RecordingFileChooser(
                ServiceProvider* service_provider,
                ui::FocusHelper& focus_helper,
                const ui::Layout& layout,
                bool is_top_level
        );

        OOPETRIS_GRAPHICS_EXPORTED void render(const ServiceProvider& service_provider) const override;

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] Widget::EventHandleResult
        handle_event(const std::shared_ptr<input::InputManager>& input_manager, const SDL_Event& event) override;

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] const std::vector<std::filesystem::path>& get_currently_chosen_files(
        ) const;

    private:
        [[nodiscard]] std::tuple<ui::Label*, ui::Label*, ui::Label*, ui::Label*> get_texts();

        void populate_texts();

        void prepare_dialog(ServiceProvider* service_provider);
        void cleanup_dialog(ServiceProvider* service_provider);
    };


} // namespace custom_ui
