#pragma once


#include "helper/export_symbols.hpp"
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
    ServiceProvider* m_service_provider;

public:
    OOPETRIS_GRAPHICS_EXPORTED explicit SettingsManager(ServiceProvider* service_provider);

    [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED const settings::Settings& settings() const;

    OOPETRIS_GRAPHICS_EXPORTED void add_callback(Callback&& callback);

    OOPETRIS_GRAPHICS_EXPORTED void save() const;

    OOPETRIS_GRAPHICS_EXPORTED void save(const settings::Settings& new_settings);

private:
    void fire_callbacks() const;
};
