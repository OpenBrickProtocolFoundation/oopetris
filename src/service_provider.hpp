#pragma once

struct CommandLineArguments;
struct Settings;
struct MusicManager;
struct Renderer;
struct FontManager;
struct EventDispatcher;
struct Window;

struct ServiceProvider {
    virtual ~ServiceProvider() = default;

    [[nodiscard]] virtual CommandLineArguments& command_line_arguments() = 0;
    [[nodiscard]] virtual const CommandLineArguments& command_line_arguments() const = 0;
    [[nodiscard]] virtual Settings& settings() = 0;
    [[nodiscard]] virtual const Settings& settings() const = 0;
    [[nodiscard]] virtual MusicManager& music_manager() = 0;
    [[nodiscard]] virtual const MusicManager& music_manager() const = 0;
    [[nodiscard]] virtual const Renderer& renderer() const = 0;
    [[nodiscard]] virtual FontManager& fonts() = 0;
    [[nodiscard]] virtual const FontManager& fonts() const = 0;
    [[nodiscard]] virtual EventDispatcher& event_dispatcher() = 0;
    [[nodiscard]] virtual const EventDispatcher& event_dispatcher() const = 0;
    [[nodiscard]] virtual const Window& window() const = 0;
};
