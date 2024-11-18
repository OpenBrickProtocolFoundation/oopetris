#pragma once

#include <core/helper/types.hpp>

#include "graphics/renderer.hpp"
#include "graphics/window.hpp"
#include "input/input.hpp"
#include "lobby/api.hpp"
#include "manager/event_dispatcher.hpp"
#include "manager/event_listener.hpp"
#include "manager/music_manager.hpp"
#include "manager/resource_manager.hpp"
#include "manager/service_provider.hpp"
#include "manager/settings_manager.hpp"
#include "scenes/loading_screen/loading_screen.hpp"
#include "scenes/scene.hpp"
#include "ui/components/label.hpp"

#include <chrono>
#include <future>
#include <memory>
#include <vector>

namespace helper {
#if !defined(NDEBUG)
    struct DebugInfo {
        Uint64 m_start_time;
        u64 m_frame_counter;

    private:
        Uint64 m_update_time;
        double m_count_per_s;

    public:
        DebugInfo(Uint64 start_time, u64 frame_counter, Uint64 update_time, double count_per_s);

        [[nodiscard]] Uint64 update_time() const;
        [[nodiscard]] double count_per_s() const;
    };

#endif
    struct TimeInfo {
    private:
        std::chrono::nanoseconds m_sleep_time;

    public:
        std::chrono::steady_clock::time_point m_start_execution_time;

        TimeInfo(std::chrono::nanoseconds sleep_time, std::chrono::steady_clock::time_point start_execution_time);

        [[nodiscard]] std::chrono::nanoseconds sleep_time() const;
    };

    struct LoadingInfo {
    private:
        std::chrono::nanoseconds m_sleep_time;
        Uint64 m_start_time;

        std::future<void> m_load_everything_thread;

    public:
        std::chrono::steady_clock::time_point m_start_execution_time;
        bool m_finished_loading;
        scenes::LoadingScreen m_loading_screen;

        LoadingInfo(
                std::chrono::nanoseconds sleep_time,
                Uint64 start_time,
                std::future<void>&& load_everything_thread,
                std::chrono::steady_clock::time_point start_execution_time,
                bool finished_loading,
                scenes::LoadingScreen&& loading_screen
        );

        [[nodiscard]] std::chrono::nanoseconds sleep_time() const;

        [[nodiscard]] Uint64 start_time() const;

        [[nodiscard]] const std::future<void>& load_everything_thread() const;
    };
} // namespace helper


struct Application final : public EventListener, public ServiceProvider {
private:
    static constexpr auto num_audio_channels = u8{ 2 };

    bool m_is_running{ true };
    CommandLineArguments m_command_line_arguments;
    std::shared_ptr<Window> m_window;
    Renderer m_renderer;
    std::optional<u32> m_target_framerate;

    // these fields are initalized asynchronously in a separate thread
    std::unique_ptr<SettingsManager> m_settings_manager;
    std::unique_ptr<MusicManager> m_music_manager;
    std::shared_ptr<input::InputManager> m_input_manager;
    std::unique_ptr<FontManager> m_font_manager;
    std::unique_ptr<lobby::API> m_api;

#if defined(__EMSCRIPTEN__)
    using EmscriptenFunction = std::function<void()>;
    EmscriptenFunction m_current_emscripten_func;

    void load_emscripten();
    void main_loop_emscripten();
#endif


#if !defined(NDEBUG)
    std::unique_ptr<ui::Label> m_fps_text{ nullptr };
    std::unique_ptr<helper::DebugInfo> m_debug;
#endif
    std::unique_ptr<helper::TimeInfo> m_time_info;
    std::unique_ptr<helper::LoadingInfo> m_loading_info;

#if defined(_HAVE_DISCORD_SDK)
    std::optional<DiscordInstance> m_discord_instance{ std::nullopt };
#endif

protected:
    EventDispatcher
            m_event_dispatcher; //NOLINT(misc-non-private-member-variables-in-classes,cppcoreguidelines-non-private-member-variables-in-classes)

private:
    std::vector<std::unique_ptr<scenes::Scene>> m_scene_stack;

    void main_loop();

    void load_loop();

public:
    Application(std::shared_ptr<Window>&& window, CommandLineArguments&& arguments);
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    void run();

    void handle_event(const SDL_Event& event) override;

    virtual void update();

    virtual void render() const;

#if defined(__EMSCRIPTEN__)
    void loop_entry_emscripten();
#endif

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

    [[nodiscard]] const std::unique_ptr<lobby::API>& api() const override {
        return m_api;
    }


#if defined(_HAVE_DISCORD_SDK)

    [[nodiscard]] std::optional<DiscordInstance>& discord_instance() override;
    [[nodiscard]] const std::optional<DiscordInstance>& discord_instance() const override;


#endif


private:
    void initialize();
    void load_resources();
    void reload_api(const settings::Settings& settings);
};
