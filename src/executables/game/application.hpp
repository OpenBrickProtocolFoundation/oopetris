#pragma once

#include <core/helper/types.hpp>

#include "graphics/renderer.hpp"
#include "graphics/window.hpp"
#include "input/input.hpp"
#include "manager/event_dispatcher.hpp"
#include "manager/event_listener.hpp"
#include "manager/music_manager.hpp"
#include "manager/resource_manager.hpp"
#include "manager/service_provider.hpp"
#include "manager/settings_manager.hpp"
#include "scenes/scene.hpp"
#include "ui/components/label.hpp"
#include "lobby/api.hpp"

#include <memory>
#include <vector>

struct Application final : public EventListener, public ServiceProvider {
private:
    static constexpr auto num_audio_channels = u8{ 2 };

    bool m_is_running{ true };
    CommandLineArguments m_command_line_arguments;
    std::shared_ptr<Window> m_window;
    Renderer m_renderer;
    std::optional<u32> m_target_framerate;

    // these fields are initalized asynchronously in a separate thread
    std::unique_ptr<MusicManager> m_music_manager;
    std::shared_ptr<input::InputManager> m_input_manager;
    std::unique_ptr<SettingsManager> m_settings_manager;
    std::unique_ptr<FontManager> m_font_manager;
    std::unique_ptr<lobby::API> m_api;


#if !defined(NDEBUG)
    std::unique_ptr<ui::Label> m_fps_text{ nullptr };
#endif

#if defined(_HAVE_DISCORD_SDK)
    std::optional<DiscordInstance> m_discord_instance{ std::nullopt };
#endif

protected:
    EventDispatcher m_event_dispatcher;

private:
    std::vector<std::unique_ptr<scenes::Scene>> m_scene_stack;

public:
    Application(std::shared_ptr<Window>&& window, CommandLineArguments&& arguments);
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    void run();

    void handle_event(const SDL_Event& event) override;

    virtual void update();
    virtual void render() const;

    //TODO(Totto): move those functions bodies to the cpp

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

    FontManager& font_manager() override {
        return *m_font_manager;
    }

    [[nodiscard]] const FontManager& font_manager() const override {
        return *m_font_manager;
    }

    CommandLineArguments& command_line_arguments() override {
        return m_command_line_arguments;
    }

    [[nodiscard]] const CommandLineArguments& command_line_arguments() const override {
        return m_command_line_arguments;
    }

    SettingsManager& settings_manager() override {
        return *m_settings_manager;
    }

    [[nodiscard]] const SettingsManager& settings_manager() const override {
        return *m_settings_manager;
    }

    MusicManager& music_manager() override {
        return *m_music_manager;
    }

    [[nodiscard]] const MusicManager& music_manager() const override {
        return *m_music_manager;
    }

    [[nodiscard]] const Renderer& renderer() const override {
        return m_renderer;
    }

    [[nodiscard]] const Window& window() const override {
        return *m_window;
    }

    [[nodiscard]] Window& window() override {
        return *m_window;
    }

    [[nodiscard]] input::InputManager& input_manager() override {
        return *m_input_manager;
    }

    [[nodiscard]] const input::InputManager& input_manager() const override {
        return *m_input_manager;
    }


#if defined(_HAVE_DISCORD_SDK)

    [[nodiscard]] std::optional<DiscordInstance>& discord_instance() override;
    [[nodiscard]] const std::optional<DiscordInstance>& discord_instance() const override;


#endif


private:
    void initialize();
    void load_resources();
};
