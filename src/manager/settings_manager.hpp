#pragma once


#include "helper/magic_enum_wrapper.hpp"
#include "helper/parse_json.hpp"
#include "helper/platform.hpp"
#include "input/keyboard_input.hpp"
#include "manager/service_provider.hpp"

#include <fmt/format.h>
#include <string>
#include <variant>


using Controls = std::variant<input::KeyboardSettings>;

inline void to_json(nlohmann::json& j, const Controls& controls) {
    std::visit(
            helper::overloaded{
                    [&](const input::KeyboardSettings& keyboard_controls) {
                        to_json(j, keyboard_controls);
                        j["type"] = "keyboard";
                    },
            },
            controls
    );
}

inline void from_json(const nlohmann::json& j, Controls& controls) {
    const auto& type = j.at("type");

    if (type == "keyboard") {
        input::KeyboardSettings keyboard_controls{};
        from_json(j, keyboard_controls);
        controls = keyboard_controls;
    } else {
        throw std::runtime_error{ fmt::format("unsupported control type '{}'", to_string(type)) };
    }
}


namespace detail {

    static constexpr auto settings_filename = "settings.json";


    struct Settings {
        Controls controls;
        float volume{ 0.2f };
        bool discord{ false }; //changing this requires a restart
    };


    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Settings, volume, discord)

} // namespace detail


struct SettingsManager {
private:
    ServiceProvider* m_service_provider;
    detail::Settings m_current_settings;

public:
    explicit SettingsManager(ServiceProvider* service_provider);

    [[nodiscard]] const detail::Settings& settings() const;
};
