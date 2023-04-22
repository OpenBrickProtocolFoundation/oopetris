#include "utils.hpp"
#include "constants.hpp"
#include <SDL.h>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <string>

namespace utils {
    [[nodiscard]] std::string current_date_time_iso8601() {
        auto now = std::chrono::system_clock::now();

        std::time_t time = std::chrono::system_clock::to_time_t(now);

        char buffer[16];

        std::tm tm{};
#ifdef _MSC_VER
        if (gmtime_s(&tm, &time) != 0) {
            spdlog::error("error calling gmtime_s");
            return "error";
        }
#else
        if (gmtime_r(&time, &tm) == nullptr) {
            spdlog::error("error calling gmtime_r");
            return "error";
        }
#endif
        std::strftime(buffer, sizeof(buffer), "%Y%m%dT%H%M%S", &tm);

        return std::string{ buffer };
    }

    [[nodiscard]] std::filesystem::path get_root_folder() {
#if defined(__ANDROID__)
        char* pref_path = SDL_GetPrefPath(constants::author, constants::program_name);
        if (!pref_path) {
            throw std::runtime_error{ "Failed in getting the Pref Path on android!" };
        }
        return std::filesystem::path{ std::string{ pref_path } };
#elif defined(__SWITCH__)
        // this is in the sdcard of the switch, since internal storage is read-only for applications!
        return std::filesystem::path{ "." };
#else
        return std::filesystem::path{ "." };
#endif
    }

    [[nodiscard]] std::filesystem::path get_assets_folder() {
#if defined(__ANDROID__)
        return std::filesystem::path{ "." };
#elif defined(__SWITCH__)
        // this is in the internal storage of the nintendo switch, it ios mounted by libnx (runtime switch support library) and filled at compile time with assets (its called ROMFS there)
        return std::filesystem::path{ "romfs:/assets" };
#else
        return std::filesystem::path{ "assets" };
#endif
    }


    [[nodiscard]] std::filesystem::path get_subfolder_to_root(const std::string_view folder) {
        return get_root_folder() / folder;
    }


} // namespace utils
