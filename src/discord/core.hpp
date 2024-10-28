

#pragma once

#include <core/helper/expected.hpp>
#include <core/helper/types.hpp>

#include "../helper/windows.hpp"

#include <chrono>
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#endif

#include <discord.h>
#include <memory>
#include <string>

namespace constants::discord {
    constexpr auto client_id = 1220147916371394650ULL;

    //TODO(Totto):  this isn't correct for all platforms and needs to be tested
#if defined(__ANDROID__)
    constexpr const char* platform_dependent_launch_arguments = "";
#elif defined(__CONSOLE__)
#error "Not supported"
#elif defined(FLATPAK_BUILD)
    constexpr const char* platform_dependent_launch_arguments =
            "flatpak run io.github.openbrickprotocolfoundation.oopetris --discord";
#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    constexpr const char* platform_dependent_launch_arguments = "oopetris.exe --discord";
#elif defined(__APPLE__)
    constexpr const char* platform_dependent_launch_arguments = "TODO";
#elif defined(__linux__)
    constexpr const char* platform_dependent_launch_arguments = "oopetris --discord";
#else
#error "Unsupported platform"
#endif


#if defined(__ANDROID__)
    constexpr const std::uint32_t supported_platforms = DiscordActivitySupportedPlatformFlags_Android;
#elif defined(__CONSOLE__)
#error "Not supported"
#elif defined(FLATPAK_BUILD) || defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) \
        || defined(__APPLE__) || defined(__linux__)
    constexpr const std::uint32_t supported_platforms = DiscordActivitySupportedPlatformFlags_Desktop;
#else
#error "Unsupported platform"
#endif


    // manually synchronized to https://discord.com/developers/applications/1220147916371394650/rich-presence/assets
    enum class ArtAsset : u8 { Logo };

    OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] std::string get_asset_key(ArtAsset asset);

} // namespace constants::discord


struct DiscordActivityWrapper {
private:
    discord::Activity m_activity{};

public:
    //TODO(Totto):  Add support for party and invites / join / invitations / spectate

    OOPETRIS_GRAPHICS_EXPORTED DiscordActivityWrapper(const std::string& details, discord::ActivityType type);

    OOPETRIS_GRAPHICS_EXPORTED DiscordActivityWrapper&
    set_large_image(const std::string& text, constants::discord::ArtAsset asset);

    OOPETRIS_GRAPHICS_EXPORTED DiscordActivityWrapper&
    set_small_image(const std::string& text, constants::discord::ArtAsset asset);

    OOPETRIS_GRAPHICS_EXPORTED DiscordActivityWrapper& set_details(const std::string& text);

    template<typename T>
    DiscordActivityWrapper& set_start_timestamp(const std::chrono::time_point<T>& point) {

        const auto seconds_since_epoch = static_cast<discord::Timestamp>(
                std::chrono::duration_cast<std::chrono::seconds>(point.time_since_epoch()).count()
        );

        m_activity.GetTimestamps().SetStart(seconds_since_epoch);

        return *this;
    }

    template<typename T>
    DiscordActivityWrapper& set_end_timestamp(const std::chrono::time_point<T>& point) {

        const auto seconds_since_epoch = static_cast<discord::Timestamp>(
                std::chrono::duration_cast<std::chrono::seconds>(point.time_since_epoch()).count()
        );

        m_activity.GetTimestamps().SetEnd(seconds_since_epoch);

        return *this;
    }


    OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] const discord::Activity& get_raw() const;
};

struct DiscordInstance {
private:
    std::unique_ptr<discord::Core> m_core;
    std::unique_ptr<discord::User> m_current_user;

    DiscordInstance(discord::Core* core);

public:
    OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] static helper::expected<DiscordInstance, std::string> initialize();

    OOPETRIS_GRAPHICS_EXPORTED void after_setup();

    OOPETRIS_GRAPHICS_EXPORTED DiscordInstance(DiscordInstance&& old) noexcept;

    OOPETRIS_GRAPHICS_EXPORTED DiscordInstance& operator=(DiscordInstance&& other) noexcept;

    OOPETRIS_GRAPHICS_EXPORTED ~DiscordInstance();

    OOPETRIS_GRAPHICS_EXPORTED void update();
    OOPETRIS_GRAPHICS_EXPORTED void set_activity(const DiscordActivityWrapper& activity);

private:
    void clear_activity(bool wait = true);
};
