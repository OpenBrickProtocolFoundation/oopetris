#include "application.hpp"
#include "helper/errors.hpp"
#include "helper/message_box.hpp"
#include "helper/utils.hpp"

#include <filesystem>
#include <fmt/format.h>
#include <memory>

#if defined(__ANDROID__)
#include <spdlog/sinks/android_sink.h>
#endif

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_sinks.h>

#if defined(__SWITCH__)
#include "switch.h"
#include <string.h>
#endif


int main(int argc, char** argv) {
    const auto logs_path = utils::get_root_folder() / "logs";
    if (not std::filesystem::exists(logs_path)) {
        std::filesystem::create_directory(logs_path);
    }

    std::vector<spdlog::sink_ptr> sinks;
#if defined(__ANDROID__)
    sinks.push_back(std::make_shared<spdlog::sinks::android_sink_mt>());
#else
    sinks.push_back(std::make_shared<spdlog::sinks::stdout_sink_st>());
#endif
    sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            fmt::format("{}/oopetris.log", logs_path.string()), 1024 * 1024 * 10, 5, true
    ));
    auto combined_logger = std::make_shared<spdlog::logger>("combined_logger", begin(sinks), end(sinks));
    spdlog::set_default_logger(combined_logger);

#ifdef DEBUG_BUILD
    spdlog::set_level(spdlog::level::debug);
#else
    spdlog::set_level(spdlog::level::err);
#endif

    std::vector<std::string> arguments{};
    arguments.reserve(argc);
    for (auto i = 0; i < argc; ++i) {
        arguments.emplace_back(argv[i]); //NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    }

    if (arguments.empty()) {
        arguments.emplace_back("oopetris");
    }

    constexpr auto window_name = constants::program_name.c_str();

    std::unique_ptr<Window> window{ nullptr };

    try {
#if defined(__ANDROID__) or defined(__SWITCH__)
        window = std::make_unique<Window>(window_name, WindowPosition::Centered);
#else
        static constexpr int width = 1280;
        static constexpr int height = 720;

        window = std::make_unique<Window>(window_name, WindowPosition::Centered, width, height);
#endif
    } catch (const helper::GeneralError& general_error) {
        spdlog::error("{}", general_error.message());
    }

    if (window == nullptr) {
        helper::MessageBox::show_simple(
                helper::MessageBox::Type::Error, "Initialization Error", "failed to create SDL window", nullptr
        );
        return EXIT_FAILURE;
    }


    try {
        Application app{ std::move(window), arguments };

        app.run();
        return EXIT_SUCCESS;
    } catch (const helper::GeneralError& general_error) {

        spdlog::error("{}", general_error.message());
        return EXIT_FAILURE;
    }
}
