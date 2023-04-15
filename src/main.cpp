#include "command_line_arguments.hpp"
#include "tetris_application.hpp"
#include <filesystem>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/stdout_sinks.h>

int main(int argc, char** argv) {
    const auto logs_path = std::filesystem::path{ "logs" };
    if (not exists(logs_path)) {
        std::filesystem::create_directory(logs_path);
    }
    const auto log_filename = fmt::format("{}/{}.log", logs_path.string(), utils::current_date_time_iso8601());

    std::vector<spdlog::sink_ptr> sinks;
    sinks.push_back(std::make_shared<spdlog::sinks::stdout_sink_st>());
    sinks.push_back(std::make_shared<spdlog::sinks::daily_file_sink_st>(log_filename, 23, 59));
    auto combined_logger = std::make_shared<spdlog::logger>("combined_logger", begin(sinks), end(sinks));
    spdlog::set_default_logger(combined_logger);

#ifdef DEBUG_BUILD
    spdlog::set_level(spdlog::level::debug);
#else
    spdlog::set_level(spdlog::level::err);
#endif

    TetrisApplication tetris_app(CommandLineArguments{ argc, argv });
    tetris_app.run();

    return 0;
}
