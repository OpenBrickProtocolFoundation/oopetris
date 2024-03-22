

#pragma once

#include "helper/expected.hpp"

#include <chrono>
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

    // manually synchronized to https://discord.com/developers/applications/1220147916371394650/rich-presence/assets
    enum class ArtAsset { logo };

    [[nodiscard]] std::string get_asset_key(ArtAsset asset);


} // namespace constants::discord


struct DiscordActivityWrapper {
private:
    discord::Activity m_activity{};

public:
    //TODO: Add support for party and invites / join / invitations / spectate

    DiscordActivityWrapper(const std::string& details, discord::ActivityType type);

    DiscordActivityWrapper& set_large_image(const std::string& text, constants::discord::ArtAsset asset);
    DiscordActivityWrapper& set_small_image(const std::string& text, constants::discord::ArtAsset asset);
    DiscordActivityWrapper& set_details(const std::string& text);

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


    [[nodiscard]] const discord::Activity& get_raw() const;
};

struct DiscordInstance {
private:
    std::unique_ptr<discord::Core> m_core;
    std::unique_ptr<discord::User> m_current_user;

    DiscordInstance(discord::Core* core);

public:
    [[nodiscard]] static helper::expected<DiscordInstance, std::string> initialize();

    void after_setup();

    DiscordInstance(DiscordInstance&& old) noexcept;
    DiscordInstance& operator=(DiscordInstance&& other) noexcept;

    ~DiscordInstance();

    void update();
    void set_activity(const DiscordActivityWrapper& activity);

private:
    void clear_activity(bool wait = true);
};
