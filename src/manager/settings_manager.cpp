#include "settings_manager.hpp"
#include "helper/graphic_utils.hpp"
#include "input/keyboard_input.hpp"
#include "input/touch_input.hpp"

namespace {
    constexpr const auto settings_filename = "settings.json";

}


SettingsManager::SettingsManager() {
    const std::filesystem::path settings_file = utils::get_root_folder() / settings_filename;

    const auto result = json::try_parse_json_file<settings::Settings>(settings_file);

    if (result.has_value()) {
        m_settings = result.value();
    } else {
        auto [error, error_type] = result.error();

        spdlog::error("unable to load settings from \"{}\": {}", settings_filename, error);
        spdlog::warn("applying default settings");

        m_settings = {
            settings::Settings{ .controls = {},
                               .selected = std::nullopt,
                               .volume = 1.0,
                               .discord = false,
                               .api_url = std::nullopt }
        };

        //save the default file, only if it doesn't exist, if it has an error, just leave it there
        if (error_type == json::ParseError::OpenError) {
            this->save();
        }
    }
}


[[nodiscard]] const settings::Settings& SettingsManager::settings() const {
    return m_settings;
}


void SettingsManager::add_callback(Callback&& callback) {
    m_callbacks.emplace_back(std::move(callback));
}

void SettingsManager::save() const {
    const std::filesystem::path settings_file = utils::get_root_folder() / settings_filename;

    const auto result = json::try_write_json_to_file(settings_file, m_settings, true);

    if (result.has_value()) {
        spdlog::error("unable to save settings to \"{}\": {}", settings_filename, result.value());
        return;
    }

    this->fire_callbacks();
}

void SettingsManager::save(const settings::Settings& new_settings) {
    this->m_settings = new_settings;
    this->save();
}

void SettingsManager::fire_callbacks() const {
    for (const auto& callback : m_callbacks) {
        callback(m_settings);
    }
}
