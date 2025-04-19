#pragma once

#include "manager/event_dispatcher.hpp"


#if defined(_HAVE_DISCORD_SOCIAL_SDK) && !defined(_OOPETRIS_RECORDING_UTILITY)

#include "discord/core.hpp"


#endif


//forward declarations
struct CommandLineArguments;
struct SettingsManager;
struct MusicManager;
struct EventDispatcher;
struct Renderer;
struct FontManager;
struct Window;

namespace input {
    struct InputManager;
}

namespace scenes {
    struct Scene;
}

namespace lobby {
    struct API;

}

namespace web {
    struct WebContext;
}

struct ServiceProvider {
    ServiceProvider() = default;
    ServiceProvider(const ServiceProvider&) = delete;
    ServiceProvider(ServiceProvider&&) = delete;
    ServiceProvider& operator=(const ServiceProvider&) = delete;
    ServiceProvider& operator=(ServiceProvider&&) = delete;
    virtual ~ServiceProvider() = default;

    [[nodiscard]] virtual CommandLineArguments& command_line_arguments() = 0;
    [[nodiscard]] virtual const CommandLineArguments& command_line_arguments() const = 0;
    [[nodiscard]] virtual SettingsManager& settings_manager() = 0;
    [[nodiscard]] virtual const SettingsManager& settings_manager() const = 0;
    [[nodiscard]] virtual MusicManager& music_manager() = 0;
    [[nodiscard]] virtual const MusicManager& music_manager() const = 0;
    [[nodiscard]] virtual const Renderer& renderer() const = 0;
    [[nodiscard]] virtual FontManager& font_manager() = 0;
    [[nodiscard]] virtual const FontManager& font_manager() const = 0;
    [[nodiscard]] virtual EventDispatcher& event_dispatcher() = 0;
    [[nodiscard]] virtual const EventDispatcher& event_dispatcher() const = 0;
    [[nodiscard]] virtual const Window& window() const = 0;
    [[nodiscard]] virtual Window& window() = 0;

    [[nodiscard]] virtual input::InputManager& input_manager() = 0;
    [[nodiscard]] virtual const input::InputManager& input_manager() const = 0;

    [[nodiscard]] virtual const std::unique_ptr<lobby::API>& api() const = 0;

#if defined(_HAVE_DISCORD_SOCIAL_SDK) && !defined(_OOPETRIS_RECORDING_UTILITY)

    [[nodiscard]] virtual std::optional<DiscordInstance>& discord_instance() = 0;
    [[nodiscard]] virtual const std::optional<DiscordInstance>& discord_instance() const = 0;

#endif

#if defined(__EMSCRIPTEN__)

    [[nodiscard]] virtual web::WebContext& web_context() = 0;
    [[nodiscard]] virtual const web::WebContext& web_context() const = 0;

#endif
};
