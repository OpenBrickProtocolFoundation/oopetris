#pragma once

#include "application.hpp"
#include "music_manager.hpp"
#include "recording.hpp"
#include "resource_manager.hpp"
#include "settings.hpp"
#include "tetrion.hpp"
#include "tetromino_type.hpp"
#include <cassert>
#include <fstream>
#include <span>
#include <spdlog/spdlog.h>
#include <stdexcept>

struct TetrisApplication : public Application {
private:
    static constexpr auto settings_filename = "settings.json";
    Settings m_settings;
    FontManager m_font_manager;

public:
    static constexpr int width = 1280;
    static constexpr int height = 720;

    explicit TetrisApplication(CommandLineArguments command_line_arguments);

    [[nodiscard]] const Settings& settings() const {
        return m_settings;
    }

    [[nodiscard]] const FontManager& fonts() const {
        return m_font_manager;
    }

    [[nodiscard]] FontManager& fonts() {
        return m_font_manager;
    }

private:
    void try_load_settings();

    void load_resources();
};
