#pragma once

#include "input/joystick_input.hpp"
#include "input/keyboard_input.hpp"
#include "input/touch_input.hpp"
#include "manager/service_provider.hpp"

#include <fmt/format.h>
#include <variant>

struct Controls {
private:
    using Type = std::variant<input::KeyboardSettings, input::JoystickSettings, input::TouchSettings>;
    Type m_content;

public:
    // default constructor is need for deserialization
    Controls() : m_content{} { }
    template<typename T>
    explicit Controls(T content) : m_content{ std::move(content) } {
        //
    }

    template<typename T>
    Controls& operator=(T&& content) {
        m_content = std::move(content);
        return *this;
    }

    [[nodiscard]] const Type& content() const;
};


void to_json(nlohmann::json& j, const Controls& controls);

void from_json(const nlohmann::json& j, Controls& controls);

namespace detail {

    static constexpr auto settings_filename = "settings.json";


    struct Settings {
        std::vector<Controls> controls;
        float volume{ 0.2f };
        bool discord{ false }; //changing this requires a restart
    };


    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Settings, controls, volume, discord)

} // namespace detail


struct SettingsManager {
private:
    ServiceProvider* m_service_provider;
    detail::Settings m_current_settings;

public:
    explicit SettingsManager(ServiceProvider* service_provider);

    [[nodiscard]] const detail::Settings& settings() const;
};
