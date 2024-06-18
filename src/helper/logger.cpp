

#include "logger.hpp"


#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_sinks.h>


#if defined(__ANDROID__)
#include <spdlog/sinks/android_sink.h>
#endif

#if defined(__CONSOLE__)
#include "helper/console_helpers.hpp"
#endif

#include "helper/graphic_utils.hpp"

#include <filesystem>
#include <fmt/format.h>


#if !defined(SPDLOG_THREAD_USAGE)
#define SPDLOG_THREAD_USAGE 0
#endif

#if SPDLOG_THREAD_USAGE == 0
#define SPDLOG_THREAD_DEF(x) x##_mt
#elif SPDLOG_THREAD_USAGE == 1
#define SPDLOG_THREAD_DEF(x) x##_st
#else
#error "SPDLOG_THREAD_USAGE has unknown value"
#endif


void logger::initialize() {
    console::debug_print("logger: 1\n");

/* 
    const auto logs_path = utils::get_root_folder() / "logs";
    if (not std::filesystem::exists(logs_path)) {
        std::filesystem::create_directory(utils::get_root_folder());
        std::filesystem::create_directory(logs_path);
    }
 */
    console::debug_print("logger: 2\n");

    std::vector<spdlog::sink_ptr> sinks;
#if defined(__ANDROID__)
    sinks.push_back(std::make_shared<spdlog::sinks::SPDLOG_THREAD_DEF(android_sink)>());
#elif defined(__CONSOLE__)
    console::debug_print("logger: 3\n");
    sinks.push_back(std::make_shared<console::SPDLOG_THREAD_DEF(debug_sink)>());
    console::debug_print("logger: 4\n");
#else
    sinks.push_back(std::make_shared<spdlog::sinks::SPDLOG_THREAD_DEF(stdout_sink)>());
#endif
    console::debug_print("logger: 5\n");
   /*  sinks.push_back(std::make_shared<spdlog::sinks::SPDLOG_THREAD_DEF(rotating_file_sink)>(
            fmt::format("{}/oopetris.log", logs_path.string()), 1024 * 1024 * 10, 5, true
    )); */
    console::debug_print("logger: 6\n");
    auto combined_logger = std::make_shared<spdlog::logger>("combined_logger", begin(sinks), end(sinks));
    console::debug_print("logger: 7\n");
    spdlog::set_default_logger(combined_logger);
    console::debug_print("logger: 8\n");

#if !defined(NDEBUG)
    spdlog::set_level(spdlog::level::debug);
#else
    spdlog::set_level(spdlog::level::err);
#endif
    console::debug_print("logger: 9\n");
}
