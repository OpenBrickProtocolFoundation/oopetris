#pragma once


#include "focus_layout.hpp"
#include "helper/types.hpp"

namespace ui {
    struct GridLayout : public FocusLayout {
    private:
        u32 size;
        Direction direction;
        Margin gap;
        std::pair<u32, u32> margin;

    public:
        explicit GridLayout(
                u32 focus_id,
                u32 size,
                Direction direction,
                Margin gap,
                std::pair<double, double> margin,
                const Layout& layout,
                bool is_top_level = true
        );

        [[nodiscard]] u32 total_size() const;

        void render(const ServiceProvider& service_provider) const override;

        Widget::EventHandleResult
        handle_event(const std::shared_ptr<input::InputManager>& input_manager, const SDL_Event& event) override;


    private:
        [[nodiscard]] Layout get_layout_for_index(u32 index) override;
    };

} // namespace ui
