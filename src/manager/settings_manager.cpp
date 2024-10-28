#include "settings_manager.hpp"
#include "helper/graphic_utils.hpp"
#include "input/keyboard_input.hpp"
#include "input/touch_input.hpp"

namespace {
    static constexpr auto settings_filename = "settings.json";

}


SettingsManager::SettingsManager() {
    const std::filesystem::path settings_file = utils::get_root_folder() / settings_filename;

    const auto result = json::try_parse_json_file<settings::Settings>(settings_file);

    if (result.has_value()) {
        m_settings = result.value();
    } else {
        spdlog::error("unable to load settings from \"{}\": {}", settings_filename, result.error());
        spdlog::warn("applying default settings");

        m_settings = {
            settings::Settings{ .controls = {},
                               .selected = std::nullopt,
                               .volume = 1.0,
                               .discord = false,
                               .api_url = std::nullopt }
        };

        //TODO(Totto): save the file, if it doesn't exist, if it has an error, just leave it there
    }
}


[[nodiscard]] const settings::Settings& SettingsManager::settings() const {
    return m_settings;
}


void SettingsManager::add_callback(Callback&& callback) {
    m_callbacks.emplace_back(std::move(callback));
}

void SettingsManager::save() const {

    const auto result = json::try_convert_to_json(m_settings);

    if (not result.has_value()) {
        spdlog::error("unable to convert settings to json {}", result.error());
        return;
    }

    const auto saved = this->save_to_file(result.value());
    if (saved.has_value()) {
        spdlog::error("unable to save settings from \"{}\": {}", settings_filename, saved.value());
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


std::optional<std::string> SettingsManager::save_to_file(const std::string& content) const {

    const std::filesystem::path settings_file = utils::get_root_folder() / settings_filename;

    std::ofstream file_stream{ settings_file };

    if (file_stream.is_open()) {
        return fmt::format("File '{}' couldn't be opened!", settings_file.string());
    }

    file_stream << content;

    file_stream.close();

    if (file_stream.fail()) {
        return fmt::format("Couldn't write to file '{}' ", settings_file.string());
    }

    return std::nullopt;
}
