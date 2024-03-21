

#include "core.hpp"
#include "helper/magic_enum_wrapper.hpp"
#include "types.h"

#include <fmt/format.h>
#include <spdlog/spdlog.h>

DiscordInstance::DiscordInstance(discord::Core* core) : m_core{ core }, m_current_user{} {


    m_core->UserManager().OnCurrentUserUpdate.Connect([this]() {
        this->m_current_user = std::make_unique<discord::User>();
        this->m_core->UserManager().GetCurrentUser(this->m_current_user.get());
    });


    m_core->ActivityManager().RegisterCommand(constants::discord::platform_dependent_launch_arguments);
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


void DiscordInstance::update() {
    m_core->RunCallbacks();
}
