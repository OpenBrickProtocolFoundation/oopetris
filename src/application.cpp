#include "application.hpp"
#include "scenes/scene.hpp"
#include <fstream>

Application::Application(
        int argc,
        char** argv,
        const std::string& title,
        WindowPosition position,
        int width,
        int height
)
    : m_command_line_arguments{ argc, argv },
      m_window{ title, position, width, height },
      m_renderer{ m_window, Renderer::VSync::Enabled },
      m_music_manager{ this, num_audio_channels } {
    initialize();
}

Application::Application(int argc, char** argv, const std::string& title, WindowPosition position)
    : m_command_line_arguments{ argc, argv },
      m_window{ title, position },
      m_renderer{ m_window, Renderer::VSync::Enabled },
      m_music_manager{ this, num_audio_channels } {
    initialize();
}

void Application::run() {
    m_event_dispatcher.register_listener(this);
    while (m_is_running) {
        m_event_dispatcher.dispatch_pending_events();
        update();
        render();
        m_renderer.present();
    }
}

void Application::handle_event(const SDL_Event& event) {
    if (event.type == SDL_QUIT) {
        m_is_running = false;
    }

#if defined(__ANDROID__)
    if (event.type == SDL_KEYDOWN and event.key.keysym.sym == SDLK_AC_BACK) {
        //TODO: also catch the resume event from the app (see SDLActivity.java)
        // pause()
    }
#endif

    for (usize i = 0; i < m_scene_stack.size(); ++i) {
        const auto index = m_scene_stack.size() - i - 1;
        if (m_scene_stack.at(index)->handle_event(event)) {
            break;
        }
    }
}

void Application::update() {
    // we have to cache the initial number of scenes before starting the loop since the number of scenes can
    // change during the loop which may otherwise lead to iterating over the same scene multiple times
    const auto num_scenes = m_scene_stack.size();

    for (usize i = 0; i < num_scenes; ++i) {
        const auto index = num_scenes - i - 1;
        const auto [scene_update, scene_change] = m_scene_stack.at(index)->update();
        if (scene_change) {
            std::visit(
                    overloaded{
                            [this, index](const scenes::Scene::Pop&) {
                                m_scene_stack.erase(
                                        m_scene_stack.begin()
                                        + static_cast<decltype(m_scene_stack.begin())::difference_type>(index)
                                );
                            },
                            [this](const scenes::Scene::Push& push) {
                                spdlog::info("pushing back scene {}", magic_enum::enum_name(push.target_scene));
                                m_scene_stack.push_back(scenes::create_scene(*this, push.target_scene));
                            },
                            [this](const scenes::Scene::Switch& switch_) {
                                if (not switch_.add) {
                                    m_scene_stack.clear();
                                }
                                m_scene_stack.push_back(scenes::create_scene(*this, switch_.target_scene));
                            },
                            [this](const scenes::Scene::Exit&) { m_is_running = false; },
                    },
                    *scene_change
            );
        }
        if (scene_update == scenes::SceneUpdate::StopUpdating) {
            break;
        }
    }
}

void Application::render() const {
    renderer().clear();
    for (const auto& scene : m_scene_stack) {
        scene->render(*this);
    }
}

void Application::initialize() {
    try_load_settings();
    load_resources();
    push_scene(scenes::create_scene(*this, SceneId::MainMenu));
}

void Application::try_load_settings() try {
    std::ifstream settings_file{ settings_filename };
    m_settings = nlohmann::json::parse(settings_file);
    spdlog::info("settings loaded");
} catch (...) {
    spdlog::error("unable to load settings from \"{}\"", settings_filename);
    spdlog::warn("applying default settings");
}

void Application::load_resources() {
    const auto font_path = utils::get_assets_folder() / "fonts" / "PressStart2P.ttf";
#if defined(__ANDROID__)
    constexpr auto font_size = 35;
#else
    constexpr auto font_size = 18;
#endif

    // todo: catch exception
    m_font_manager.load(FontId::Default, font_path, font_size);
}
