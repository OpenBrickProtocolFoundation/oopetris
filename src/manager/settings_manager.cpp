#include "settings_manager.hpp"
#include "helper/graphic_utils.hpp"
#include "input/keyboard_input.hpp"
#include "input/touch_input.hpp"
#include <core/helper/spdlog_wrapper.hpp>

#if defined(__EMSCRIPTEN__)
#include "helper/web_utils.hpp"
#endif

namespace {
#if defined(__EMSCRIPTEN__)
    constexpr const auto settings_key = "settings_key";
#else
    constexpr const auto settings_filename = "settings.json";
#endif

} // namespace


SettingsManager::SettingsManager(ServiceProvider* service_provider) : m_service_provider{ service_provider } {

    // To stop clang about complaining
    UNUSED(m_service_provider);
#if defined(__EMSCRIPTEN__)
    const auto content = m_service_provider->web_context().local_storage().get_item(settings_key);

    helper::expected<settings::Settings, std::pair<std::string, json::ParseError>> result =
            helper::unexpected<std::pair<std::string, json::ParseError>>{ std::make_pair<std::string, json::ParseError>(
                    "Key not present in LocalStorage", json::ParseError::OpenError
            ) };

    if (content.has_value()) {
        auto parse_result = json::try_parse_json<settings::Settings>(content.value());

        if (not parse_result.has_value()) {
            result = helper::unexpected<std::pair<std::string, json::ParseError>>{
                std::make_pair<std::string, json::ParseError>(
                        std::move(parse_result.error()), json::ParseError::FormatError
                )
            };
        } else {
            result = parse_result.value();
        }
    }

#else
    const std::filesystem::path settings_file = utils::get_root_folder() / settings_filename;

    const auto result = json::try_parse_json_file<settings::Settings>(settings_file);
#endif

    if (result.has_value()) {
        m_settings = result.value();
    } else {
        auto [error, error_type] = result.error();

        spdlog::error(
                "unable to load settings from \"{}\": {}",
#if defined(__EMSCRIPTEN__)
                settings_key,
#else
                settings_filename,
#endif
                error
        );
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

#if defined(__EMSCRIPTEN__)
    const auto maybe_settings_json = json::try_json_to_string<settings::Settings>(m_settings);

    if (not maybe_settings_json.has_value()) {
        spdlog::error(
                "unable to save settings to LocalStorage\"{}\": unable to convert settings to json: {}", settings_key,
                maybe_settings_json.error()
        );
        return;
    }

    auto is_successfull =
            m_service_provider->web_context().local_storage().set_item(settings_key, maybe_settings_json.value());

    if (not is_successfull) {
        spdlog::error("unable to save settings to LocalStorage\"{}\": localstorage set error", settings_key);
        return;
    }

#else
    const std::filesystem::path settings_file = utils::get_root_folder() / settings_filename;

    const auto result = json::try_write_json_to_file<settings::Settings>(settings_file, m_settings, true);


    if (result.has_value()) {
        spdlog::error("unable to save settings to \"{}\": {}", settings_filename, result.value());
        return;
    }
#endif


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
