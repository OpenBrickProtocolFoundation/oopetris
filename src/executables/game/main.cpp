

#include "./parser.hpp"

#include <core/helper/errors.hpp>
#include <core/helper/utils.hpp>

#include "application.hpp"
#include "helper/message_box.hpp"

#include <filesystem>
#include <fmt/format.h>
#include <memory>

#if defined(__ANDROID__)
#include <spdlog/sinks/android_sink.h>
#endif

#if defined(__CONSOLE__)
#include "helper/console_helpers.hpp"
#endif


#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_sinks.h>


int main(int argc, char** argv) {
    const auto logs_path = utils::get_root_folder() / "logs";
    if (not std::filesystem::exists(logs_path)) {
        std::filesystem::create_directory(logs_path);
    }

    std::vector<spdlog::sink_ptr> sinks;
#if defined(__ANDROID__)
    sinks.push_back(std::make_shared<spdlog::sinks::android_sink_mt>());
#elif defined(__CONSOLE__)
    sinks.push_back(std::make_shared<console::debug_sink_mt>());
#else
    sinks.push_back(std::make_shared<spdlog::sinks::stdout_sink_mt>());
#endif
    sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            fmt::format("{}/oopetris.log", logs_path.string()), 1024 * 1024 * 10, 5, true
    ));
    auto combined_logger = std::make_shared<spdlog::logger>("combined_logger", begin(sinks), end(sinks));
    spdlog::set_default_logger(combined_logger);

#if !defined(NDEBUG)
    spdlog::set_level(spdlog::level::debug);
#else
    spdlog::set_level(spdlog::level::err);
#endif

    std::vector<std::string> arguments_vector{};
    arguments_vector.reserve(argc);
    for (auto i = 0; i < argc; ++i) {
        arguments_vector.emplace_back(argv[i]); //NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    }

    if (arguments_vector.empty()) {
        arguments_vector.emplace_back("oopetris");
    }

    auto parsed_arguments = helper::parse_args(arguments_vector);

    if (not parsed_arguments.has_value()) {

        spdlog::error("error parsing command line arguments: {}", parsed_arguments.error());
#if defined(__ANDROID__)
        // calling exit() in android doesn't do the correct job, it completely avoids resource cleanup by the underlying SDLActivity.java
        // (java wrapper), that calls the main and expects it to return ALWAYS and throwing an exception in a catch statement is bad,
        // but is required here
        throw std::runtime_error{ "exit with status code 1: " + std::string{ err.what() } };
#else
        std::exit(1);
#endif
    }

    auto arguments = std::move(parsed_arguments.value());

    constexpr auto window_name = constants::program_name.c_str();

    std::shared_ptr<Window> window{ nullptr };

    try {
#if defined(__ANDROID__) or defined(__CONSOLE__)
        window = std::make_shared<Window>(window_name, WindowPosition::Centered);
#else
        static constexpr int width = 1280;
        static constexpr int height = 720;

        window = std::make_shared<Window>(window_name, WindowPosition::Centered, width, height);
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
        Application app{ std::move(window), std::move(arguments) };

        app.run();
        return EXIT_SUCCESS;
    } catch (const helper::GeneralError& general_error) {

        spdlog::error("{}", general_error.message());
        return EXIT_FAILURE;
    }
}
