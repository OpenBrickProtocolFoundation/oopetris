#pragma once


#include "helper/windows.hpp"
#include "input/controller_input.hpp"
#include "input/joystick_input.hpp"
#include "input/keyboard_input.hpp"
#include "input/touch_input.hpp"
#include "manager/service_provider.hpp"

#include <fmt/format.h>
#include <variant>

using Controls =
        std::variant<input::KeyboardSettings, input::JoystickSettings, input::TouchSettings, input::ControllerSettings>;

namespace nlohmann {
    template<>
    struct adl_serializer<Controls> {
        static Controls from_json(const json& obj) {
            const auto& type = obj.at("type");

            if (type == "keyboard") {
                return Controls{ nlohmann::adl_serializer<input::KeyboardSettings>::from_json(obj) };
            }

            if (type == "joystick") {
                return Controls{ nlohmann::adl_serializer<input::JoystickSettings>::from_json(obj) };
            }

            if (type == "controller") {
                return Controls{ nlohmann::adl_serializer<input::ControllerSettings>::from_json(obj) };
            }

            if (type == "touch") {
                return Controls{ nlohmann::adl_serializer<input::TouchSettings>::from_json(obj) };
            }

            throw std::runtime_error{ fmt::format("unsupported control type '{}'", to_string(type)) };
        }

        static void to_json(json& obj, Controls controls) { // NOLINT(misc-no-recursion)
            std::visit(
                    helper::overloaded{
                            [&](const input::KeyboardSettings& keyboard_settings) { // NOLINT(misc-no-recursion)
                                nlohmann::adl_serializer<input::KeyboardSettings>::to_json(obj, keyboard_settings);
                                obj["type"] = "keyboard";
                            },
                            [&](const input::JoystickSettings& joystick_settings) { // NOLINT(misc-no-recursion)
                                nlohmann::adl_serializer<input::JoystickSettings>::to_json(obj, joystick_settings);
                                obj["type"] = "joystick";
                            },
                            [&](const input::ControllerSettings& controller_settings) { // NOLINT(misc-no-recursion)
                                nlohmann::adl_serializer<input::ControllerSettings>::to_json(obj, controller_settings);
                                obj["type"] = "controller";
                            },
                            [&](const input::TouchSettings& touch_settings) { // NOLINT(misc-no-recursion)
                                nlohmann::adl_serializer<input::TouchSettings>::to_json(obj, touch_settings);
                                obj["type"] = "touch";
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
        std::optional<u32> selected;
        float volume{ 0.2F };
        bool discord{ false }; //changing this requires a restart
    };


    OOPETRIS_GRAPHICS_EXPORTED void to_json(nlohmann::json& obj, const Settings& settings);

    OOPETRIS_GRAPHICS_EXPORTED void from_json(const nlohmann::json& obj, Settings& settings);

} // namespace detail


struct SettingsManager {
private:
    ServiceProvider* m_service_provider;
    detail::Settings m_settings;

public:
    OOPETRIS_GRAPHICS_EXPORTED explicit SettingsManager(ServiceProvider* service_provider);

    OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] const detail::Settings& settings() const;
};
