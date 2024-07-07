
#include "./service_provider.hpp"
#include "utils/helper.hpp"

#include <gtest/gtest.h>

#if defined(__GNUC__) || defined(__clang__)
#define FUNCT __FUNCTION__
#elif
#define FUNCT ""
#endif

DummyServiceProvider::DummyServiceProvider() = default;

// implementation of ServiceProvider
[[nodiscard]] EventDispatcher& DummyServiceProvider::event_dispatcher() {
    ASSERT_FAIL(FUNCT);
}

[[nodiscard]] const EventDispatcher& DummyServiceProvider::event_dispatcher() const {
    ASSERT_FAIL(FUNCT);
}

FontManager& DummyServiceProvider::font_manager() {
    ASSERT_FAIL(FUNCT);
}

[[nodiscard]] const FontManager& DummyServiceProvider::font_manager() const {
    ASSERT_FAIL(FUNCT);
}

CommandLineArguments& DummyServiceProvider::command_line_arguments() {
    ASSERT_FAIL(FUNCT);
}

[[nodiscard]] const CommandLineArguments& DummyServiceProvider::command_line_arguments() const {
    ASSERT_FAIL(FUNCT);
}

SettingsManager& DummyServiceProvider::settings_manager() {
    ASSERT_FAIL(FUNCT);
}

[[nodiscard]] const SettingsManager& DummyServiceProvider::settings_manager() const {
    ASSERT_FAIL(FUNCT);
}

MusicManager& DummyServiceProvider::music_manager() {
    ASSERT_FAIL(FUNCT);
}

[[nodiscard]] const MusicManager& DummyServiceProvider::music_manager() const {
    ASSERT_FAIL(FUNCT);
}

[[nodiscard]] const Renderer& DummyServiceProvider::renderer() const {
    ASSERT_FAIL(FUNCT);
}

[[nodiscard]] const Window& DummyServiceProvider::window() const {
    ASSERT_FAIL(FUNCT);
}

[[nodiscard]] Window& DummyServiceProvider::window() {
    ASSERT_FAIL(FUNCT);
}

[[nodiscard]] input::InputManager& DummyServiceProvider::input_manager() {
    ASSERT_FAIL(FUNCT);
}

[[nodiscard]] const input::InputManager& DummyServiceProvider::input_manager() const {
    ASSERT_FAIL(FUNCT);
}


#if defined(_HAVE_DISCORD_SDK)

[[nodiscard]] std::optional<DiscordInstance>& DummyServiceProvider::discord_instance() {
    ASSERT_FAIL(FUNCT);
}
[[nodiscard]] const std::optional<DiscordInstance>& DummyServiceProvider::discord_instance() const {
    ASSERT_FAIL(FUNCT);
}


#endif
