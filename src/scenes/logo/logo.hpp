#pragma once


#include "graphics/texture.hpp"
#include "manager/service_provider.hpp"


namespace logo {
    constexpr const auto width = 33;
    constexpr const auto height = 5;

    OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] Texture
    get_logo(const ServiceProvider* service_provider, double scale = 1.0);

} // namespace logo
