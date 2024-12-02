

#pragma once

#include <SDL.h>
#include <memory>

#include "game/game.hpp"
#include "graphics/rect.hpp"
#include "helper/windows.hpp"
#include "manager/service_provider.hpp"
#include "renderer.hpp"

struct VideoRenderer : ServiceProvider {
private:
    std::unique_ptr<SDL_Surface> m_surface;
    std::unique_ptr<Renderer> m_renderer;
    std::vector<std::unique_ptr<Game>> m_games;
    ServiceProvider* m_main_provider;
    shapes::UPoint m_size;
    std::shared_ptr<ManualClock> m_clock;

    void initialize_games(const std::filesystem::path& recording_path);

public:
    OOPETRIS_GRAPHICS_EXPORTED explicit VideoRenderer(
            ServiceProvider* service_provider,
            const std::filesystem::path& recording_path,
            shapes::UPoint size
    );

    std::optional<std::string>
    render(const std::string& destination_path, u32 fps, const std::function<void(double progress)>& progress_callback);

    ~VideoRenderer() override;


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

    [[nodiscard]] const std::unique_ptr<lobby::API>& api() const override;

#if defined(_HAVE_DISCORD_SDK)

    [[nodiscard]] std::optional<DiscordInstance>& discord_instance() override;

    [[nodiscard]] const std::optional<DiscordInstance>& discord_instance() const override;

#endif

#if defined(__EMSCRIPTEN__)

    [[nodiscard]] web::WebContext& web_context() override;
    [[nodiscard]] const web::WebContext& web_context() const override;

#endif
};


struct Decoder;


//TODO(Totto): also support library and not only subprocess call
// See e.g. https://github.com/Raveler/ffmpeg-cpp
struct VideoRendererBackend {
private:
    std::filesystem::path m_destination_path;
    std::unique_ptr<Decoder> m_decoder;

    [[nodiscard]] static std::vector<std::string>
    get_encoding_paramaters(u32 fps, shapes::UPoint size, const std::filesystem::path& destination_path);

public:
    OOPETRIS_GRAPHICS_EXPORTED explicit VideoRendererBackend(std::filesystem::path destination_path);

    OOPETRIS_GRAPHICS_EXPORTED ~VideoRendererBackend();

    OOPETRIS_GRAPHICS_EXPORTED static void is_supported_async(const std::function<void(bool)>& callback);

    OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] std::optional<std::string> setup(u32 fps, shapes::UPoint size);

    [[nodiscard]] bool add_frame(SDL_Surface* surface);

    [[nodiscard]] bool finish(bool cancel);
};
