

#include "video_renderer.hpp"
#include "game/layout.hpp"

#include <fmt/format.h>

VideoRenderer::VideoRenderer(
        ServiceProvider* service_provider,
        const std::filesystem::path& recording_path,
        shapes::UPoint size
)
    : m_main_provider{ service_provider },
      m_size{ size } {
    auto* surface = SDL_CreateRGBSurface(0, static_cast<int>(size.x), static_cast<int>(size.y), 32, 0, 0, 0, 0);

    if (surface == nullptr) {
        throw std::runtime_error{ fmt::format("Failed creating a SDL RGB Surface: {}", SDL_GetError()) };
    }

    m_surface.reset(surface);
    auto renderer = Renderer::get_software_renderer(m_surface);

    m_renderer = std::make_unique<Renderer>(std::move(renderer));

    initialize_games(recording_path);
}

void VideoRenderer::initialize_games(const std::filesystem::path& recording_path) {

    auto [parameters, information] = input::get_game_parameters_for_replay(this, recording_path);

    auto layout = ui::FullScreenLayout{
        shapes::URect{ { 0, 0 }, m_size }
    };

    std::vector<ui::Layout> layouts = game::get_layouts_for(parameters.size(), layout);

    m_clock = std::make_shared<ManualClock>();

    for (decltype(parameters.size()) i = 0; i < parameters.size(); ++i) {
        auto [input, starting_parameters] = std::move(parameters.at(i));

        m_games.emplace_back(
                std::make_unique<Game>(this, std::move(input), starting_parameters, m_clock, layouts.at(i), false)
        );
    }
}


VideoRenderer::~VideoRenderer() {
    if (m_surface) {
        SDL_FreeSurface(m_surface.get());
    }
}


std::optional<std::string> VideoRenderer::render(
        const std::string& destination_path,
        u32 fps,
        const std::function<void(double progress)>& progress_callback
) {

    auto backend = VideoRendererBackend{ destination_path };

    if (auto result = backend.setup(fps, m_size); result.has_value()) {
        return fmt::format("No video renderer backend available: {}", result.value());
    }

    auto all_games_finished = [this]() -> bool {
        for (const auto& game : m_games) {
            if (not game->is_game_finished()) {
                return false;
            }
        }

        return true;
    };

    //TODO: this is just a dummy thing atm, change that
    double progress = 0.0;

    while (not all_games_finished()) {
        progress_callback(progress);

        m_renderer->clear();

        for (const auto& game : m_games) {
            if (not game->is_game_finished()) {
                game->update();
                game->render(*this);
            }
        }

        m_renderer->present();

        if (not backend.add_frame(m_surface.get())) {
            break;
        }

        m_clock->increment_simulation_step_index();

        progress += 0.1;

        progress_callback(progress);
    }

    if (not backend.finish(false)) {
        return "Renderer failed";
    }
    return std::nullopt;
}

std::vector<std::string> VideoRendererBackend::get_encoding_paramaters(
        u32 fps,
        shapes::UPoint size,
        const std::filesystem::path& destination_path
) {

    const std::string resolution = fmt::format("{}x{}", size.x, size.y);

    const std::string framerate = fmt::format("{}", fps);

    return {
        "-loglevel",
#if !defined(NDEBUG)
        "verbose",
#else
        "warning",
#endif
        "-y", // always overwrite video
        "-f",
        "rawvideo",
        "-pix_fmt",
        "bgra",
        "-s",
        resolution,
        "-r",
        framerate,
        "-i",
        "-",
        "-c:v",
        "libx264",
        "-crf",
        "20",
        "-pix_fmt",
        "yuv420p",
        destination_path.string(),
    };
}


// implementation of ServiceProvider

[[nodiscard]] EventDispatcher& VideoRenderer::event_dispatcher() {
    return m_main_provider->event_dispatcher();
}

[[nodiscard]] const EventDispatcher& VideoRenderer::event_dispatcher() const {
    return m_main_provider->event_dispatcher();
}

FontManager& VideoRenderer::font_manager() {
    return m_main_provider->font_manager();
}

[[nodiscard]] const FontManager& VideoRenderer::font_manager() const {
    return m_main_provider->font_manager();
}

CommandLineArguments& VideoRenderer::command_line_arguments() {
    return m_main_provider->command_line_arguments();
}

[[nodiscard]] const CommandLineArguments& VideoRenderer::command_line_arguments() const {
    return m_main_provider->command_line_arguments();
}

SettingsManager& VideoRenderer::settings_manager() {
    return m_main_provider->settings_manager();
}

[[nodiscard]] const SettingsManager& VideoRenderer::settings_manager() const {
    return m_main_provider->settings_manager();
}

MusicManager& VideoRenderer::music_manager() {
    return m_main_provider->music_manager();
}

[[nodiscard]] const MusicManager& VideoRenderer::music_manager() const {
    return m_main_provider->music_manager();
}

[[nodiscard]] const Renderer& VideoRenderer::renderer() const {
    return *m_renderer;
}

[[nodiscard]] const Window& VideoRenderer::window() const {
    return m_main_provider->window();
}

[[nodiscard]] Window& VideoRenderer::window() {
    return m_main_provider->window();
}

[[nodiscard]] input::InputManager& VideoRenderer::input_manager() {
    return m_main_provider->input_manager();
}

[[nodiscard]] const input::InputManager& VideoRenderer::input_manager() const {
    return m_main_provider->input_manager();
}

[[nodiscard]] const std::unique_ptr<lobby::API>& VideoRenderer::api() const {
    return m_main_provider->api();
}

#if defined(_HAVE_DISCORD_SDK)

[[nodiscard]] std::optional<DiscordInstance>& VideoRenderer::discord_instance() {
    return m_main_provider->discord_instance();
}

[[nodiscard]] const std::optional<DiscordInstance>& VideoRenderer::discord_instance() const {
    return m_main_provider->discord_instance();
}

#endif
