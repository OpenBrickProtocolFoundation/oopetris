#include "settings_manager.hpp"
#include "helper/graphic_utils.hpp"


#include <spdlog/spdlog.h>

SettingsManager::SettingsManager(ServiceProvider* service_provider) : m_service_provider{ service_provider } {
    const std::filesystem::path settings_file = utils::get_root_folder() / detail::settings_filename;

    const auto result = json::try_parse_json_file<detail::Settings>(settings_file);

    if (result.has_value()) {
        m_current_settings = result.value();
    } else {
        spdlog::error("unable to load settings from \"{}\": {}", detail::settings_filename, result.error());
        spdlog::warn("applying default settings");

        //TODO: better default settings
        m_current_settings = detail::Settings{ input::KeyboardSettings{}, 1.0 };
    }
}
