

#include "core.hpp"
#include "helper/magic_enum_wrapper.hpp"
#include "helper/utils.hpp"
#include "types.h"

#include <fmt/format.h>
#include <spdlog/spdlog.h>


[[nodiscard]] std::string constants::discord ::get_asset_key(constants::discord::ArtAsset asset) {

    switch (asset) {
        case ArtAsset::logo:
            return "logo";
        default:
            utils::unreachable();
    }
}

DiscordInstance::DiscordInstance(discord::Core* core) : m_core{ core }, m_current_user{ nullptr } { }

void DiscordInstance::after_setup() {
    m_core->UserManager().OnCurrentUserUpdate.Connect([this]() {
        auto* user = new discord::User; // NOLINT(cppcoreguidelines-owning-memory)
        this->m_core->UserManager().GetCurrentUser(user);
        this->m_current_user.reset(user);
        spdlog::info(
                "Current user updated: {}#{}", this->m_current_user->GetUsername(),
                this->m_current_user->GetDiscriminator()
        );
    });

    auto result = m_core->ActivityManager().RegisterCommand(constants::discord::platform_dependent_launch_arguments);
    if (result != discord::Result::Ok) {
        spdlog::warn("ActivityManager: Failed to RegisterCommand: {}", magic_enum::enum_name(result));
    };
}

[[nodiscard]] helper::expected<DiscordInstance, std::string> DiscordInstance::initialize() {

    discord::Core* core{};
    auto result = discord::Core::Create(constants::discord::client_id, DiscordCreateFlags_Default, &core);
    if (core == nullptr) {
        return helper::unexpected<std::string>{
            fmt::format("Failed to instantiate discord core: {}", magic_enum::enum_name(result))
        };
    }


    core->SetLogHook(
#ifdef DEBUG_BUILD
            discord::LogLevel::Debug
#else
            discord::LogLevel::Error
#endif
            ,
            [](discord::LogLevel level, const char* message) {
                switch (level) {
                    case discord::LogLevel::Error:
                        spdlog::error("DISCORD SDK: {}", message);
                        break;
                    case discord::LogLevel::Warn:
                        spdlog::warn("DISCORD SDK: {}", message);
                        break;
                    case discord::LogLevel::Info:
                        spdlog::info("DISCORD SDK: {}", message);
                        break;
                    case discord::LogLevel::Debug:
                        spdlog::debug("DISCORD SDK: {}", message);
                        break;
                }
            }
    );

    return DiscordInstance{ core };
}


DiscordInstance::DiscordInstance(DiscordInstance&& old) noexcept
    : m_core{ std::move(old.m_core) },
      m_current_user{ std::move(old.m_current_user) } {
    old.m_core = nullptr;
    old.m_current_user = nullptr;
}


DiscordInstance& DiscordInstance::operator=(DiscordInstance&& other) noexcept {
    if (this != &other) {

        m_core = std::move(other.m_core);
        m_current_user = std::move(other.m_current_user);

        other.m_core = nullptr;
        other.m_current_user = nullptr;
    }
    return *this;
};

DiscordInstance::~DiscordInstance() {
    if (m_core != nullptr) {
        clear_activity();
    }
}

void DiscordInstance::update() {
    m_core->RunCallbacks();
}


void DiscordInstance::set_activity(const DiscordActivityWrapper& activity) {

    m_core->ActivityManager().UpdateActivity(activity.get_raw(), [](discord::Result result) {
        spdlog::info("Result to UpdateActivity: {}", magic_enum::enum_name(result));
    });
}


void DiscordInstance::clear_activity(bool wait) {
    bool received_callback = false;
    m_core->ActivityManager().ClearActivity([&received_callback](discord::Result result) {
        spdlog::info("Result to ClearActivity: {}", magic_enum::enum_name(result));
        received_callback = true;
    });

    if (wait) {
        while (not received_callback) {
            this->update();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}


DiscordActivityWrapper::DiscordActivityWrapper(const std::string& details, discord::ActivityType type) {


    m_activity.SetDetails(details.c_str());
    m_activity.SetType(type);
}


DiscordActivityWrapper&
DiscordActivityWrapper::add_large_image(const std::string& text, constants::discord::ArtAsset asset) {
    m_activity.GetAssets().SetLargeText(text.c_str());

    const auto asset_key = constants::discord::get_asset_key(asset);
    m_activity.GetAssets().SetLargeImage(asset_key.c_str());

    return *this;
}


DiscordActivityWrapper&
DiscordActivityWrapper::add_small_image(const std::string& text, constants::discord::ArtAsset asset) {
    m_activity.GetAssets().SetSmallText(text.c_str());

    const auto asset_key = constants::discord::get_asset_key(asset);
    m_activity.GetAssets().SetSmallImage(asset_key.c_str());
    return *this;
}

DiscordActivityWrapper& DiscordActivityWrapper::set_details(const std::string& text) {
    m_activity.SetState(text.c_str());

    return *this;
}


[[nodiscard]] const discord::Activity& DiscordActivityWrapper::get_raw() const {
    return m_activity;
}
