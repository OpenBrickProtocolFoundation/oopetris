#pragma once

#include "scenes/scene.hpp"
#include "ui/components/button.hpp"
#include "ui/layouts/grid_layout.hpp"
#include "ui/components/label.hpp"
#include "ui/components/slider.hpp"

namespace scenes {

    struct AboutPage : public Scene {
    private:
        bool m_should_exit = false;
        ui::GridLayout<5> m_main_grid;


    public:
        explicit AboutPage(ServiceProvider* service_provider, const ui::Layout& layout);

        [[nodiscard]] UpdateResult update() override;
        void render(const ServiceProvider& service_provider) override;
        bool handle_event(const SDL_Event& event, const Window* window) override;
    };

} // namespace scenes
