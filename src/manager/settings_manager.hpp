#pragma once


#include "helper/windows.hpp"
#include "input/controller_input.hpp"
#include "input/joystick_input.hpp"
#include "input/keyboard_input.hpp"
#include "input/touch_input.hpp"
#include "manager/service_provider.hpp"

#include "./settings.hpp"


struct SettingsManager {
public:
    using Callback = std::function<void(const settings::Settings& settings)>;

private:
    settings::Settings m_settings;
    std::vector<Callback> m_callbacks;

public:
    OOPETRIS_GRAPHICS_EXPORTED explicit SettingsManager();

    OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] const settings::Settings& settings() const;

    OOPETRIS_GRAPHICS_EXPORTED void add_callback(Callback&& callback);

    OOPETRIS_GRAPHICS_EXPORTED void save() const;

    OOPETRIS_GRAPHICS_EXPORTED void save(const settings::Settings& new_settings);

private:
    void fire_callbacks() const;
    std::optional<std::string> save_to_file(const std::string& content) const;
};
