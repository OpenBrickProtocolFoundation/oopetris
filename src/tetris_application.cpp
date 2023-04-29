#include "tetris_application.hpp"
#include "ingame_scene.hpp"
#include "main_menu_scene.hpp"
#include "utils.hpp"
#include <tl/optional.hpp>

TetrisApplication::TetrisApplication(CommandLineArguments command_line_arguments)
#if defined(__ANDROID__)
    : Application{ "OOPetris", WindowPosition::Centered, std::move(command_line_arguments) } {
#else
    : Application{ "OOPetris", WindowPosition::Centered, width, height, std::move(command_line_arguments) } {
#endif

    try_load_settings();
    load_resources();
    push_scene(std::make_unique<IngameScene>(this));
    push_scene(std::make_unique<MainMenuScene>(this));
}

void TetrisApplication::try_load_settings() try {
    std::ifstream settings_file{ settings_filename };
    m_settings = nlohmann::json::parse(settings_file);
    spdlog::info("settings loaded");
} catch (...) {
    spdlog::error("unable to load settings from \"{}\"", settings_filename);
    spdlog::warn("applying default settings");
}

void TetrisApplication::load_resources() {
    const auto font_path = utils::get_assets_folder() / "fonts" / "PressStart2P.ttf";
#if defined(__ANDROID__)
    constexpr auto font_size = 35;
#else
    constexpr auto font_size = 18;
#endif

    // todo: catch exception
    m_font_manager.load("default", font_path, font_size);
}
