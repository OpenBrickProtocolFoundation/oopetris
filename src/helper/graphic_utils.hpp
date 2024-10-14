
#pragma once

#include <core/helper/color.hpp>

#include "./windows.hpp"
#include "helper/constants.hpp"

#include <SDL.h>
#include <spdlog/spdlog.h>

namespace utils {

    OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] SDL_Color sdl_color_from_color(const Color& color);

    OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] std::vector<std::string> supported_features();

    OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] std::filesystem::path get_assets_folder();

    OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] std::filesystem::path get_root_folder();

    OOPETRIS_GRAPHICS_EXPORTED std::optional<bool> log_error(const std::string& error);

    struct ExitException : std::exception {
    private:
        int m_status_code;

    public:
        OOPETRIS_GRAPHICS_EXPORTED explicit ExitException(int status_code) noexcept;

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] int status_code() const;

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] const char* what() const noexcept override;
    };


// this needs some special handling, so the macro is defined here
#if defined(_MSC_VER)
#if defined(OOPETRIS_LIBRARY_GRAPHICS_TYPE) && OOPETRIS_LIBRARY_GRAPHICS_TYPE == 0

#else

#endif
#else

#endif


#if defined(_MSC_VER)
#if defined(OOPETRIS_LIBRARY_GRAPHICS_TYPE) && OOPETRIS_LIBRARY_GRAPHICS_TYPE == 0
#if defined(OOPETRIS_LIBRARY_GRAPHICS_EXPORT)
#define OOPETRIS_GRAPHICS_EXPORTED_NORETURN __declspec(dllexport, noreturn)
#else
#define OOPETRIS_GRAPHICS_EXPORTED_NORETURN __declspec(dllimport, noreturn)
#endif
#else
#define OOPETRIS_GRAPHICS_EXPORTED_NORETURN __declspec(noreturn)
#endif
#else
#define OOPETRIS_GRAPHICS_EXPORTED_NORETURN [[noreturn]]
#endif


    OOPETRIS_GRAPHICS_EXPORTED_NORETURN void exit(int status_code);


#undef OOPETRIS_GRAPHICS_EXPORTED_NORETURN

} // namespace utils
