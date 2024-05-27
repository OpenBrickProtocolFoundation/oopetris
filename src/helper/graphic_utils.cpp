
#include "graphic_utils.hpp"
#include <exception>

SDL_Color utils::sdl_color_from_color(const Color& color) {
    return SDL_Color{ color.r, color.g, color.b, color.a };
}


std::vector<std::string> utils::supported_features() {

    std::vector<std::string> features{};

#if !defined(_ONLINE_MULTIPLAYER_NOT_SUPPORTED)
    features.emplace_back("online multiplayer");
#endif

#if defined(_HAVE_FILE_DIALOGS)
    features.emplace_back("file dialogs");
#endif

#if defined(_HAVE_DISCORD_SDK)
    features.emplace_back("discord integration");
#endif

    return features;
}

/**
 * \brief this returns the directory of the app, it's read and write-able
*/
[[nodiscard]] std::filesystem::path utils::get_root_folder() {
#if defined(__ANDROID__)
    // this call also creates the dir (at least tries to) it returns
    char* pref_path = SDL_GetPrefPath(constants::author, constants::program_name);
    if (!pref_path) {
        throw std::runtime_error{ "Failed in getting the Pref Path: " + std::string{ SDL_GetError() } };
    }
    return std::filesystem::path{ std::string{ pref_path } };
#elif defined(__CONSOLE__)
    // this is in the sdcard of the switch / 3ds , since internal storage is read-only for applications!
    return std::filesystem::path{ "." };
#elif defined(BUILD_INSTALLER)
#if defined(FLATPAK_BUILD)
    // this is a read write location in the flatpak build, see https://docs.flatpak.org/en/latest/conventions.html
    const char* data_home = std::getenv("XDG_DATA_HOME");
    if (data_home == nullptr) {
        throw std::runtime_error{ "Failed to get flatpak data directory (XDG_DATA_HOME)" };
    }

    return std::filesystem::path{ data_home };
#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    char* pref_path = SDL_GetPrefPath(constants::author, constants::program_name);
    if (!pref_path) {
        throw std::runtime_error{ "Failed in getting the Pref Path: " + std::string{ SDL_GetError() } };
    }
    return std::filesystem::path{ pref_path };
#else
#error "unrecognized installer build"
#endif
#else
    // this is only used in local build for debugging, when compiling in release mode the path is real path where the app can store many things without interfering with other things (eg. AppData\Roaming\... on Windows or  .local/share/... on Linux )
    return std::filesystem::path{ "." };
#endif
}

/**
 * \brief this returns the directory of the assets, and is read-only 
*/
[[nodiscard]] std::filesystem::path utils::get_assets_folder() {
#if defined(__ANDROID__)
    return std::filesystem::path{ "" };
#elif defined(__CONSOLE__)
    // this is in the internal storage of the nintendo switch, it ios mounted by libnx (runtime switch support library) and filled at compile time with assets (its called ROMFS there)
    return std::filesystem::path{ "romfs:/assets" };
#elif defined(BUILD_INSTALLER)
    // if you build in BUILD_INSTALLER mode, you have to assure that the data is there e.g. music  + fonts!
#if defined(FLATPAK_BUILD)
    return std::filesystem::path{ "/app/share/oopetris/assets/" };
#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    char* resource_path = SDL_GetBasePath();
    if (!resource_path) {
        throw std::runtime_error{ "Failed in getting the Base Path: " + std::string{ SDL_GetError() } };
    }
    return std::filesystem::path{ resource_path } / "assets";
#else
#error "unrecognized installer build"
#endif
#else
    return std::filesystem::path{ "assets" };
#endif
}

std::optional<bool> utils::log_error(const std::string& error) {
    spdlog::error(error);
    return std::nullopt;
}

utils::ExitException::ExitException(int status_code) noexcept : m_status_code{ status_code } { }

[[nodiscard]] int utils::ExitException::status_code() const {
    return m_status_code;
}

[[nodiscard]] const char* utils::ExitException::what() const noexcept {
    return "An exit exception occurred";
}

void utils::exit(int status_code) {
#if defined(__ANDROID__)
    // calling exit() in android doesn't do the correct job, it completely avoids resource cleanup by the underlying SDLActivity.java
    // (java wrapper), that calls the main and expects it to return ALWAYS and throwing an exception in a catch statement is bad,
    // but is required here
    // see: https://github.com/libsdl-org/SDL/blob/main/docs/README-android.md
    throw utils::ExitException{ status_code };
#else
    std::exit(status_code);
#endif
}
