#include "application.hpp"
#include "platform/capabilities.hpp"
#include "scenes/scene.hpp"

#include <fstream>
#include <stdexcept>

#if defined(__SWITCH__)
#include "switch.h"
#endif

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
      m_music_manager{ this, num_audio_channels },
      m_event_dispatcher{ &m_window } {
    initialize();
}

Application::Application(int argc, char** argv, const std::string& title, WindowPosition position)
    : m_command_line_arguments{ argc, argv },
      m_window{ title, position },
      m_renderer{ m_window, Renderer::VSync::Enabled },
      m_music_manager{ this, num_audio_channels },
      m_event_dispatcher{ &m_window } {
    initialize();
}

void Application::run() {
    m_event_dispatcher.register_listener(this);
    while (m_is_running
#if defined(__SWITCH__)
           // see https://switchbrew.github.io/libnx/applet_8h.html#a7ed640e5f4a81ed3960c763fdc1521c5
           // this checks for some other reasons why this app should quit, its switch specific
           and appletMainLoop()
#endif

    ) {
        m_event_dispatcher.dispatch_pending_events();
        update();
        render();
        m_renderer.present();
    }
}

void Application::handle_event(const SDL_Event& event, const Window* window) {
    if (event.type == SDL_QUIT) {
        m_is_running = false;
    }

    for (usize i = 0; i < m_scene_stack.size(); ++i) {
        const auto index = m_scene_stack.size() - i - 1;
        const auto& widget = m_scene_stack.at(index);
        if (widget->handle_event(event, window)) {
            return;
        }

        // detect if the scene overlaps everything, if that's the case, break out of the loop, since no other scene should receive inputs, since it's not visible to the user

        if (widget->get_layout().is_full_screen()) {
            break;
        }

        // if the scene is not covering the whole screen, it should give scenes in the background mouse events, but keyboard events are still only captured by the scene in focus
        if (utils::event_is_click_event(event, utils::CrossPlatformClickEvent::Any)) {
            if (utils::is_event_in(window, event, widget->get_layout().get_rect())) {
                break;
            }
        }
    }


    // this global event handlers (atm only one) are special cases, they receive all inputs if they are not handled by the scene explicably

    if (m_music_manager.handle_event(event)) {
        return;
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
                    helpers::overloaded{
                            [this, index](const scenes::Scene::Pop&) {
                                m_scene_stack.erase(
                                        m_scene_stack.begin()
                                        + static_cast<decltype(m_scene_stack.begin())::difference_type>(index)
                                );
                            },
                            [this](const scenes::Scene::Push& push) {
                                spdlog::info("pushing back scene {}", magic_enum::enum_name(push.target_scene));
                                m_scene_stack.push_back(scenes::create_scene(*this, push.target_scene, push.layout));
                            },
                            [this](const scenes::Scene::Switch& switch_) {
                                spdlog::info("switching to scene {}", magic_enum::enum_name(switch_.target_scene));
                                m_scene_stack.clear();
                                m_scene_stack.push_back(
                                        scenes::create_scene(*this, switch_.target_scene, switch_.layout)
                                );
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
    push_scene(scenes::create_scene(*this, SceneId::MainMenu, ui::FullScreenLayout{ m_window }));
}

void Application::try_load_settings() {
    const std::filesystem::path settings_file = utils::get_root_folder() / settings_filename;

    const auto result = json::try_parse_json_file<Settings>(settings_file);

    if (result.has_value()) {
        m_settings = result.value();
    } else {
        spdlog::error("unable to load settings from \"{}\": {}", settings_filename, result.error());
        spdlog::warn("applying default settings");
    }
}

void Application::load_resources() {
    const auto font_path = utils::get_assets_folder() / "fonts" / "PressStart2P.ttf";
    constexpr auto font_size = 128;
    m_font_manager.load(FontId::Default, font_path, font_size);
}

[[nodiscard]] std::vector<scenes::Scene*> Application::active_scenes() const {
    auto result = std::vector<scenes::Scene*>{};
    for (const auto& scene : m_scene_stack) {
        result.push_back(scene.get());
    }
    return result;
}
