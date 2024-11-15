

#include "./parser.hpp"

#include <core/helper/errors.hpp>
#include <core/helper/utils.hpp>

#include "application.hpp"
#include "helper/constants.hpp"
#include "helper/graphic_utils.hpp"
#include "helper/message_box.hpp"

#include <exception>
#include <filesystem>
#include <fmt/format.h>
#include <memory>
#include <vector>

#if defined(__ANDROID__)
#include <spdlog/sinks/android_sink.h>
#endif

#if defined(__CONSOLE__)
#include "helper/console_helpers.hpp"
#endif

#if defined(__EMSCRIPTEN__)
#include "helper/web_utils.hpp"
#endif

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_sinks.h>


namespace {
    void initialize_spdlog() {

        std::vector<spdlog::sink_ptr> sinks;
#if defined(__ANDROID__)
        sinks.push_back(std::make_shared<spdlog::sinks::android_sink_mt>());
#elif defined(__CONSOLE__)
        sinks.push_back(std::make_shared<console::debug_sink_mt>());
#elif defined(__EMSCRIPTEN__)
        sinks.push_back(web::get_console_sink());
#else
        sinks.push_back(std::make_shared<spdlog::sinks::stdout_sink_mt>());
#endif


#if !(defined(__EMSCRIPTEN__))

        const auto logs_path = utils::get_root_folder() / "logs";

        auto created_log_dir = utils::create_directory(logs_path, true);
        if (created_log_dir.has_value()) {
            std::cerr << "warning: couldn't create logs directory '" << logs_path.string()
                      << "': disabled file logger\n";
        }


        if (not created_log_dir.has_value()) {
            sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
                    fmt::format("{}/oopetris.log", logs_path.string()), 1024 * 1024 * 10, 5, true
            ));
        }
#endif

        auto combined_logger = std::make_shared<spdlog::logger>("combined_logger", begin(sinks), end(sinks));
        spdlog::set_default_logger(combined_logger);

#if !defined(NDEBUG)
        spdlog::set_level(spdlog::level::debug);
#else
        spdlog::set_level(spdlog::level::err);
#endif
    }


    int main_no_sdl_replace(int argc, char** argv) noexcept {

        std::shared_ptr<Window> window{ nullptr };

        try {

            initialize_spdlog();

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
                return EXIT_FAILURE;
            }

            auto arguments = std::move(parsed_arguments.value());

            [[maybe_unused]] constexpr auto window_name = constants::program_name.c_str();


            try {
#if defined(__ANDROID__) or defined(__CONSOLE__) or defined(__SERENITY__) or defined(__EMSCRIPTEN__)
                window = std::make_shared<Window>(window_name, WindowPosition::Centered);
#else
                [[maybe_unused]] static constexpr int width = 1280;
                [[maybe_unused]] static constexpr int height = 720;
                window = std::make_shared<Window>(window_name, WindowPosition::Centered, width, height);
#endif
            } catch (const helper::GeneralError& general_error) {
                spdlog::error("Couldn't initialize window: {}", general_error.message());
            }

            if (window == nullptr) {
                helper::MessageBox::show_simple(
                        helper::MessageBox::Type::Error, "Initialization Error", "failed to create SDL window", nullptr
                );
                return EXIT_FAILURE;
            }

            Application app{ std::move(window), std::move(arguments) };

            app.run();
            return EXIT_SUCCESS;

        } catch (const helper::GeneralError& general_error) {
            spdlog::error("{}", general_error.message());


            if (window == nullptr) {
                helper::MessageBox::show_simple(
                        helper::MessageBox::Type::Error, "Initialization Error", general_error.message(), nullptr
                );
            } else {
                window->show_simple(helper::MessageBox::Type::Error, "Initialization Error", general_error.message());
            }


            return EXIT_FAILURE;
        } catch (const utils::ExitException& exit_exception) {
            spdlog::debug("Requested exit with status code {}", exit_exception.status_code());
            return exit_exception.status_code();
        } catch (const std::exception& error) {
            // this is the last resort, so using std::cerr and no sdl show_simple messagebox!
            std::cerr << error.what();
            return EXIT_FAILURE;
        }
    }


} // namespace


int main(int argc, char** argv) {
    return main_no_sdl_replace(argc, argv);
}
