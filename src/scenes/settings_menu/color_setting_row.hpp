#pragma once


#include "scenes/scene.hpp"
#include "settings_details.hpp"
#include "ui/components/color_picker.hpp"
#include "ui/layouts/tile_layout.hpp"

namespace detail {


    struct ColorSettingRectangle : public ui::Widget, public ui::Focusable, public ui::Hoverable {
    private:
        Color m_color;
        shapes::URect m_fill_rect;

        explicit ColorSettingRectangle(
                const Color& start_color,
                const shapes::URect& fill_rect,
                const ui::Layout& layout,
                bool is_top_level
        );

    public:
        OOPETRIS_GRAPHICS_EXPORTED explicit ColorSettingRectangle(
                const Color& start_color,
                std::pair<double, double> size,
                ui::Alignment alignment,
                const ui::Layout& layout,
                bool is_top_level
        );

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] Color& color();

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] const Color& color() const;

        OOPETRIS_GRAPHICS_EXPORTED void render(const ServiceProvider& service_provider) const override;

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] Widget::EventHandleResult
        handle_event(const std::shared_ptr<input::InputManager>& input_manager, const SDL_Event& event) override;
    };


    struct ColorPickerScene : public scenes::Scene {
        using Callback = ui::ColorPicker::Callback;

    private:
        bool m_should_exit{ false };
        ui::ColorPicker m_color_picker;

    public:
        OOPETRIS_GRAPHICS_EXPORTED explicit ColorPickerScene(
                ServiceProvider* service_provider,
                const ui::Layout& layout,
                const Color& starting_color,
                Callback callback
        );

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] scenes::Scene::UpdateResult update() override;

        OOPETRIS_GRAPHICS_EXPORTED void render(const ServiceProvider& service_provider) override;

        OOPETRIS_GRAPHICS_EXPORTED bool
        handle_event(const std::shared_ptr<input::InputManager>& input_manager, const SDL_Event& event) override;
    };


} // namespace detail


namespace custom_ui {

    struct ColorSettingRow final : public ui::Widget,
                                   public ui::Focusable,
                                   public ui::Hoverable,
                                   public settings::SettingsDetails {
        using Callback = ui::ColorPicker::Callback;

    private:
        ServiceProvider* m_service_provider;
        ui::TileLayout m_main_layout;
        Callback m_outer_callback;

    public:
        explicit ColorSettingRow(
                ServiceProvider* service_provider,
                std::string name,
                const Color& start_color,
                Callback callback,
                u32 focus_id,
                const ui::Layout& layout,
                bool is_top_level
        );

        void render(const ServiceProvider& service_provider) const override;

        [[nodiscard]] Widget::EventHandleResult
        handle_event(const std::shared_ptr<input::InputManager>& input_manager, const SDL_Event& event) override;

        [[nodiscard]] scenes::Scene::Change get_details_scene() override;

    private:
        void inner_callback(const Color& color);

        [[nodiscard]] detail::ColorSettingRectangle* color_rect();

        [[nodiscard]] const detail::ColorSettingRectangle* color_rect() const;

        void on_focus() override;

        void on_unfocus() override;
    };


} // namespace custom_ui
