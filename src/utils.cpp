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
#else
        return std::filesystem::path{ "." };
#endif
    }

    [[nodiscard]] std::filesystem::path get_assets_folder() {
#if defined(__ANDROID__)
        return std::filesystem::path{ "." };
#else
        return std::filesystem::path{ "assets" };
#endif
    }


} // namespace utils
