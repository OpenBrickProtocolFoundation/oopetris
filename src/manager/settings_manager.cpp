#include "settings_manager.hpp"
#include "helper/graphic_utils.hpp"
#include "input/keyboard_input.hpp"
#include "input/touch_input.hpp"


#include <spdlog/spdlog.h>

SettingsManager::SettingsManager(ServiceProvider* service_provider) : m_service_provider{ service_provider } {
    const std::filesystem::path settings_file = utils::get_root_folder() / detail::settings_filename;

    const auto result = json::try_parse_json_file<detail::Settings>(settings_file);

    if (result.has_value()) {
        m_settings = result.value();
    } else {
        spdlog::error("unable to load settings from \"{}\": {}", detail::settings_filename, result.error());
        spdlog::warn("applying default settings");

        m_settings = {
            detail::Settings{ { input::KeyboardSettings::default_settings(), input::TouchSettings::default_settings() },
                             1.0 }
        };
    }
}


[[nodiscard]] const detail::Settings& SettingsManager::settings() const {
    return m_settings;
}
