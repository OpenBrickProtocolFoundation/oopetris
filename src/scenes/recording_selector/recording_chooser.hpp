

#pragma once

#include "ui/components/label.hpp"
#include "ui/focusable.hpp"
#include "ui/hoverable.hpp"
#include "ui/layouts/grid_layout.hpp"
#include "ui/widget.hpp"


namespace custom_ui {


    struct RecordingFileChooser final : public ui::Widget, public ui::Focusable, public ui::Hoverable {
    private:
        ui::GridLayout m_main_grid;

    public:
        explicit RecordingFileChooser(
                ServiceProvider* service_provider,
                ui::FocusHelper& focus_helper,
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
