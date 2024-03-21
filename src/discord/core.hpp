

#pragma once

#include "helper/expected.hpp"

#include <discord.h>
#include <memory>
#include <string>

namespace constants::discord {
    constexpr auto client_id = 1220147916371394650ULL;

    //TODO: this isn't correct for all platforms and needs to be tested
#if defined(__ANDROID__)
    constexpr const char* platform_dependent_launch_arguments = "";
#elif defined(__SWITCH__)
#error "Not supported"
#elif defined(FLATPAK_BUILD)
    constexpr const char* platform_dependent_launch_arguments = "flatpak run com.github.mgerhold.OOPetris --discord";
#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    constexpr const char* platform_dependent_launch_arguments = "oopetris.exe --discord";
#elif defined(__APPLE__)
    constexpr const char* platform_dependent_launch_arguments = "TODO";
#elif defined(__linux__)
    constexpr const char* platform_dependent_launch_arguments = "oopetris --discord";
#else
#error "Unsupported platform"
#endif

} // namespace constants::discord

struct DiscordInstance {
private:
    std::unique_ptr<discord::Core> m_core;
    std::unique_ptr<discord::User> m_current_user;

    DiscordInstance(discord::Core* core);

public:
    [[nodiscard]] static helper::expected<DiscordInstance, std::string> initialize();

    void update();
};
