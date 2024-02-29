#pragma once

#include "graphics/renderer.hpp"
#include "graphics/sdl_context.hpp"
#include "graphics/window.hpp"
#include "helper/command_line_arguments.hpp"
#include "helper/types.hpp"
#include "manager/event_dispatcher.hpp"
#include "manager/event_listener.hpp"
#include "manager/music_manager.hpp"
#include "manager/resource_manager.hpp"
#include "manager/service_provider.hpp"
#include "scenes/scene.hpp"

#ifdef DEBUG_BUILD
#include "graphics/text.hpp"
#endif

#include <memory>
#include <vector>

struct Application final : public EventListener, public ServiceProvider {
private:
    static constexpr auto num_audio_channels = u8{ 2 };

    CommandLineArguments m_command_line_arguments;
    SdlContext context;
    Window m_window;
    Renderer m_renderer;
    bool m_is_running{ true };
    MusicManager m_music_manager;
    static constexpr auto settings_filename = "settings.json";
    Settings m_settings;
    FontManager m_font_manager;
#ifdef DEBUG_BUILD
    std::unique_ptr<Text> m_fps_text{ nullptr };
#endif

protected:
    EventDispatcher m_event_dispatcher;

private:
    std::vector<std::unique_ptr<scenes::Scene>> m_scene_stack;

public:
    Application(int argc, char** argv, const std::string& title, WindowPosition position, u32 width, u32 height);
    Application(int argc, char** argv, const std::string& title, WindowPosition position);
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    void run();

    void handle_event(const SDL_Event& event, const Window* window) override;

    virtual void update();
    virtual void render() const;

    void push_scene(std::unique_ptr<scenes::Scene> scene) {
        m_scene_stack.push_back(std::move(scene));
    }

    // implementation of ServiceProvider
    [[nodiscard]] EventDispatcher& event_dispatcher() override {
        return m_event_dispatcher;
    }

    [[nodiscard]] const EventDispatcher& event_dispatcher() const override {
        return m_event_dispatcher;
    }

    FontManager& fonts() override {
        return m_font_manager;
    }
    const FontManager& fonts() const override {
        return m_font_manager;
    }

    CommandLineArguments& command_line_arguments() override {
        return m_command_line_arguments;
    }
    const CommandLineArguments& command_line_arguments() const override {
        return m_command_line_arguments;
    }
    Settings& settings() override {
        return m_settings;
    }
    const Settings& settings() const override {
        return m_settings;
    }
    MusicManager& music_manager() override {
        return m_music_manager;
    }
    const MusicManager& music_manager() const override {
        return m_music_manager;
    }

    [[nodiscard]] const Renderer& renderer() const override {
        return m_renderer;
    }

    [[nodiscard]] const Window& window() const override {
        return m_window;
    }

    [[nodiscard]] Window& window() override {
        return m_window;
    }

private:
    void initialize();
    void try_load_settings();
    void load_resources();
};
