#include "helper/utils.hpp"

#include <SDL.h>
#include <filesystem>
#include <spdlog/spdlog.h>
#include <string>

#if defined(__ANDROID__) or defined(BUILD_INSTALLER) or defined(FLATPAK_BUILD)
#include "helper/constants.hpp"
#include "helper/optional.hpp"
#endif
namespace utils {

    [[nodiscard]] std::filesystem::path get_root_folder() {
#if defined(__ANDROID__) or defined(BUILD_INSTALLER)
        // this call also creates the dir (at least tries to) it returns
        char* pref_path = SDL_GetPrefPath(constants::author, constants::program_name);
        if (!pref_path) {
            throw std::runtime_error{ "Failed in getting the Pref Path: " + std::string{ SDL_GetError() } };
        }
        return std::filesystem::path{ std::string{ pref_path } };
#elif defined(__SWITCH__)
        // this is in the sdcard of the switch, since internal storage is read-only for applications!
        return std::filesystem::path{ "." };

#elif defined(FLATPAK_BUILD)
        // this is a read write location in the flatpak build, see https://docs.flatpak.org/en/latest/conventions.html
        const char* data_home = std::getenv("XDG_DATA_HOME");
        if (data_home == = nullptr) {
            throw std::runtime_error{ "Failed to get flatpak data directory (XDG_DATA_HOME)" };
        }

        return std::filesystem::path{ data_home };
#else
        // this is only used in local build for debugging, when compiling in release mode the path is real path where the app can store many things without interfering with other things (eg. AppData\Roaming\... on Windows or  .local/share/... on Linux )
        return std::filesystem::path{ "." };
#endif
    }

    [[nodiscard]] std::filesystem::path get_assets_folder() {
#if defined(__ANDROID__)
        return std::filesystem::path{ "" };
#elif defined(__SWITCH__)
        // this is in the internal storage of the nintendo switch, it ios mounted by libnx (runtime switch support library) and filled at compile time with assets (its called ROMFS there)
        return std::filesystem::path{ "romfs:/assets" };
#elif defined(BUILD_INSTALLER)

#if defined(FLATPAK_BUILD)
        const char* resource_path = "/app/share/oopetris/";
#else
        char* resource_path = SDL_GetPrefPath(constants::author, constants::program_name);
        if (!resource_path) {
            throw std::runtime_error{ "Failed in getting the Pref Path: " + std::string{ SDL_GetError() } };
        }
// if you build in BUILD_INSTALLER mode, you have to assure that the data is there eg. music  + fonts!
#endif
        return std::filesystem::path{ std::string{ resource_path } } / "assets";
#else
        return std::filesystem::path{ "assets" };
#endif
    }

    helper::optional<bool> log_error(const std::string& error) {
        spdlog::error(error);
        return helper::nullopt;
    }

    std::vector<std::string> supported_features() {

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

} // namespace utils
