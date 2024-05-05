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
        m_current_settings = {
            detail::Settings{ { Controls{ input::KeyboardSettings{} } }, 1.0 }
        };
    }
}


void to_json(nlohmann::json& j, const Controls& controls) {
    std::visit(
            helper::overloaded{ [&](const input::KeyboardSettings& keyboard_settings) {
                                   to_json(j, keyboard_settings);
                                   j["type"] = "keyboard";
                               },
                                [&](const input::JoystickSettings& joystick_settings) {
                                    to_json(j, joystick_settings);
                                    j["type"] = "joystick";
                                },
                                [&](const input::TouchSettings& touch_settings) {
                                    to_json(j, touch_settings);
                                    j["type"] = "touch";
                                } },
            controls.content()
    );
}


void from_json(const nlohmann::json& j, Controls& controls) {
    const auto& type = j.at("type");

    if (type == "keyboard") {
        input::KeyboardSettings keyboard_settings{};
        from_json(j, keyboard_settings);
        controls = std::move(keyboard_settings);
    } else if (type == "joystick") {
        input::JoystickSettings joystick_settings{};
        from_json(j, joystick_settings);
        controls = std::move(joystick_settings);
    } else if (type == "touch") {
        input::TouchSettings touch_settings{};
        from_json(j, touch_settings);
        controls = std::move(touch_settings);
    } else {
        throw std::runtime_error{ fmt::format("unsupported control type '{}'", to_string(type)) };
    }

    // security check, since we allow an empty variant here, since its required by the json interface to be default constructible
    if (controls.content().valueless_by_exception()) {
        throw std::runtime_error{ "invalid variant state: valueless" };
    }
}
