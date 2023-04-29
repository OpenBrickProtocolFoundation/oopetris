#include "application.hpp"
#include "tetris_application.hpp"

Application::Application(
        const std::string& title,
        WindowPosition position,
        int width,
        int height,
        CommandLineArguments command_line_arguments
)
    : m_window{ title, position, width, height },
      m_renderer{ m_window },
      m_command_line_arguments{ std::move(command_line_arguments) } { }

Application::Application(
        const std::string& title,
        int x,
        int y,
        int width,
        int height,
        CommandLineArguments command_line_arguments
)
    : m_window{ title, x, y, width, height },
      m_renderer{ m_window },
      m_command_line_arguments{ std::move(command_line_arguments) } { }

Application::Application(const std::string& title, WindowPosition position, CommandLineArguments command_line_arguments)
    : m_window{ title, position },
      m_renderer{ m_window },
      m_command_line_arguments{ std::move(command_line_arguments) } { }

Application::Application(
        const std::string& title,
        int x,
        int y,
        CommandLineArguments command_line_arguments
)
    : m_window{ title, x, y, },
      m_renderer{ m_window },
      m_command_line_arguments{ std::move(command_line_arguments) } { }

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
                            [this, index](const Scene::Pop&) {
                                m_scene_stack.erase(
                                        m_scene_stack.begin()
                                        + static_cast<decltype(m_scene_stack.begin())::difference_type>(index)
                                );
                            },
                            [this](const Scene::Push& push) {
                                // todo: get rid of this ugly dynamic cast! the scene stack should work with an
                                //       application directly or with an interface type that specifies everything
                                //       that's needed
                                assert(dynamic_cast<TetrisApplication*>(this) != nullptr);
                                spdlog::info("pushing back scene {}", magic_enum::enum_name(push.target_scene));
                                m_scene_stack.push_back(
                                        create_scene(*dynamic_cast<TetrisApplication*>(this), push.target_scene)
                                );
                            },
                            [this]([[maybe_unused]] const Scene::Switch& switch_) {
                                // todo
                            },
                            [this](const Scene::Exit&) { m_is_running = false; },
                    },
                    *scene_change
            );
        }
        if (scene_update == SceneUpdate::StopUpdating) {
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
