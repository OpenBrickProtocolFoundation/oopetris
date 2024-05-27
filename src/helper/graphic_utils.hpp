
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

    struct ExitException : std::exception {
    private:
        int m_status_code;

    public:
        explicit ExitException(int status_code) noexcept;

        [[nodiscard]] int status_code() const;

        [[nodiscard]] const char* what() const noexcept override;
    };


    [[noreturn]] void exit(int status_code);

} // namespace utils
