#pragma once


#include "graphics/texture.hpp"
#include "manager/service_provider.hpp"


namespace utils {
    [[nodiscard]] Texture get_logo(const ServiceProvider* service_provider, double scale = 1.0);

}
