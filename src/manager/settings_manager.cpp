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
        spdlog::warn("unable to load settings from \"{}\": {}", detail::settings_filename, result.error());
        spdlog::warn("applying default settings");

        m_settings = {
            detail::Settings{ .controls = {}, .selected = std::nullopt, .volume = 1.0, .discord = false }
        };

        //TODO(Totto): save the file, if it doesn't exist, if it has an error, just leave it there
    }
}


[[nodiscard]] const detail::Settings& SettingsManager::settings() const {
    return m_settings;
}


void detail::to_json(nlohmann::json& obj, const detail::Settings& settings) {
    obj = nlohmann::json{
        { "controls",
         nlohmann::json{ { "inputs", settings.controls }, { "selected", settings.selected } },
         { "volume", settings.volume },
         { "discord", settings.discord } }
    };
}

void detail::from_json(const nlohmann::json& obj, detail::Settings& settings) {

    ::json::check_for_no_additional_keys(obj, { "controls", "volume", "discord" });

    obj.at("volume").get_to(settings.volume);
    obj.at("discord").get_to(settings.discord);

    const auto& controls = obj.at("controls");

    ::json::check_for_no_additional_keys(controls, { "inputs", "selected" });

    controls.at("inputs").get_to(settings.controls);
    controls.at("selected").get_to(settings.selected);
}
