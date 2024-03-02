#pragma once

#include "scenes/scene.hpp"
#include "ui/components/button.hpp"
#include "ui/components/label.hpp"
#include "ui/components/slider.hpp"
#include "ui/layouts/grid_layout.hpp"

namespace scenes {

    struct AIGame : public Scene {
    private:
        bool m_should_exit = false;
        ui::GridLayout m_main_grid;

    public:
        explicit AIGame(ServiceProvider* service_provider, const ui::Layout& layout);

        [[nodiscard]] UpdateResult update() override;
        void render(const ServiceProvider& service_provider) override;
        bool handle_event(const SDL_Event& event, const Window* window) override;
    };

} // namespace scenes
