#pragma once

#include "manager/event_dispatcher.hpp"
#include "manager/music_manager.hpp"
#include "manager/service_provider.hpp"
#include "manager/settings_manager.hpp"

struct DummyServiceProvider final : public ServiceProvider {
public:
    DummyServiceProvider();

    // implementation of ServiceProvider
    [[nodiscard]] EventDispatcher& event_dispatcher() override;

    [[nodiscard]] const EventDispatcher& event_dispatcher() const override;

    FontManager& font_manager() override;

    [[nodiscard]] const FontManager& font_manager() const override;

    CommandLineArguments& command_line_arguments() override;

    [[nodiscard]] const CommandLineArguments& command_line_arguments() const override;

    SettingsManager& settings_manager() override;

    [[nodiscard]] const SettingsManager& settings_manager() const override;

    MusicManager& music_manager() override;

    [[nodiscard]] const MusicManager& music_manager() const override;

    [[nodiscard]] const Renderer& renderer() const override;

    [[nodiscard]] const Window& window() const override;

    [[nodiscard]] Window& window() override;

    [[nodiscard]] input::InputManager& input_manager() override;

    [[nodiscard]] const input::InputManager& input_manager() const override;


#if defined(_HAVE_DISCORD_SDK)

    [[nodiscard]] std::optional<DiscordInstance>& discord_instance() override;
    [[nodiscard]] const std::optional<DiscordInstance>& discord_instance() const override;


#endif
};
