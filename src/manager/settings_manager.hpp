#pragma once

#include "helper/optional.hpp"
#include "input/joystick_input.hpp"
#include "input/keyboard_input.hpp"
#include "input/touch_input.hpp"
#include "manager/service_provider.hpp"

#include <fmt/format.h>
#include <variant>

using Controls = std::variant<input::KeyboardSettings, input::JoystickSettings, input::TouchSettings>;

namespace nlohmann {
    template<>
    struct adl_serializer<Controls> {
        static Controls from_json(const json& j) {
            const auto& type = j.at("type");

            if (type == "keyboard") {
                return Controls{ nlohmann::adl_serializer<input::KeyboardSettings>::from_json(j) };
            } else if (type == "joystick") {
                return Controls{ nlohmann::adl_serializer<input::JoystickSettings>::from_json(j) };
            } else if (type == "touch") {
                return Controls{ nlohmann::adl_serializer<input::TouchSettings>::from_json(j) };
            } else {
                throw std::runtime_error{ fmt::format("unsupported control type '{}'", to_string(type)) };
            }
        }

        static void to_json(json& j, Controls controls) {
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
                    controls
            );
        }
    };
} // namespace nlohmann


namespace detail {

    static constexpr auto settings_filename = "settings.json";


    struct Settings {
        std::vector<Controls> controls;
        helper::optional<u32> selected;
        float volume{ 0.2F };
        bool discord{ false }; //changing this requires a restart
    };


    void to_json(nlohmann::json& obj, const Settings& settings);

    void from_json(const nlohmann::json& obj, Settings& settings);

} // namespace detail


struct SettingsManager {
private:
    ServiceProvider* m_service_provider;
    detail::Settings m_settings;

public:
    explicit SettingsManager(ServiceProvider* service_provider);

    [[nodiscard]] const detail::Settings& settings() const;
};
