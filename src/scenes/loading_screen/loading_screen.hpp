#pragma once

#include "game/mino.hpp"

#include "manager/service_provider.hpp"

#include <vector>

namespace scenes {

    struct LoadingScreen {
    private:
        std::vector<std::tuple<Mino, double>> m_segments{};

        shapes::UPoint m_tile_size;
        shapes::UPoint m_start_offset;

    public:
        explicit LoadingScreen(ServiceProvider* service_provider);

        void update();

        void render(const ServiceProvider& service_provider) const;

    private:
        [[nodiscard]] shapes::UPoint to_screen_coords(const Mino::GridPoint& point) const;
    };

} // namespace scenes
