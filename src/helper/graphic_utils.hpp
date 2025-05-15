
#pragma once

#include <core/helper/color.hpp>

#include "./export_symbols.hpp"
#include "helper/constants.hpp"

#include <SDL.h>
#include <filesystem>
#include <vector>

namespace utils {

    [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED SDL_Color sdl_color_from_color(const Color& color);

    [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED std::vector<std::string> supported_features();

    [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED std::filesystem::path get_assets_folder();

    [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED std::filesystem::path get_root_folder();

    OOPETRIS_GRAPHICS_EXPORTED std::optional<bool> log_error(const std::string& error);

    struct ExitException : std::exception {
    private:
        int m_status_code;

    public:
        OOPETRIS_GRAPHICS_EXPORTED explicit ExitException(int status_code) noexcept;

        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED int status_code() const;

        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED const char* what() const noexcept override;
    };


    [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED std::optional<std::string>
    create_directory(const std::filesystem::path& folder, bool recursive);

// this needs some special handling, so the macro is defined here
#if defined(_MSC_VER)
#if defined(OOPETRIS_LIBRARY_GRAPHICS_TYPE) && OOPETRIS_LIBRARY_GRAPHICS_TYPE == 0

#else

#endif
#else

#endif


#if defined(_MSC_VER) || defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#if defined(OOPETRIS_LIBRARY_GRAPHICS_TYPE) && OOPETRIS_LIBRARY_GRAPHICS_TYPE == 0
#if defined(OOPETRIS_LIBRARY_GRAPHICS_EXPORT)
#if defined(__GNUC__)
#define OOPETRIS_GRAPHICS_EXPORTED_NORETURN __attribute__((dllexport, noreturn))
#else
#define OOPETRIS_GRAPHICS_EXPORTED_NORETURN __declspec(dllexport, noreturn)
#endif
#else
#if defined(__GNUC__)
#define OOPETRIS_GRAPHICS_EXPORTED_NORETURN __attribute__((dllimport, noreturn))
#else
#define OOPETRIS_GRAPHICS_EXPORTED_NORETURN __declspec(dllimport, noreturn)
#endif
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
