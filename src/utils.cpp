#include "utils.hpp"
#include "spdlog/spdlog.h"
#include <chrono>
#include <ctime>

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
} // namespace utils
