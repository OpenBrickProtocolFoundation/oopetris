
#pragma once

#include <core/helper/color.hpp>

#include "helper/constants.hpp"


#include <SDL.h>
#include <spdlog/spdlog.h>

namespace utils {

    [[nodiscard]] SDL_Color sdl_color_from_color(const Color& color);

    [[nodiscard]] std::vector<std::string> supported_features();

    [[nodiscard]] std::filesystem::path get_assets_folder();

    [[nodiscard]] std::filesystem::path get_root_folder();

    std::optional<bool> log_error(const std::string& error);
} // namespace utils
