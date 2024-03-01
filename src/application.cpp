#include "application.hpp"
#include "helper/sleep.hpp"
#include "platform/capabilities.hpp"
#include "scenes/scene.hpp"

#include <fstream>
#include <ranges>
#include <stdexcept>

#if defined(__SWITCH__)
#include "switch.h"
#endif

Application::Application(
        int argc,
        char** argv,
        const std::string& title,
        WindowPosition position,
        u32 width,
        u32 height
)
    : m_command_line_arguments{ argc, argv },
      m_window{ title, position, width, height },
      m_renderer{ m_window, m_command_line_arguments.target_fps.has_value() ? Renderer::VSync::Disabled
                                                                            : Renderer::VSync::Enabled },
      m_music_manager{ this, num_audio_channels },
      m_target_framerate{ m_command_line_arguments.target_fps },
      m_event_dispatcher{ &m_window } {
    initialize();
}

Application::Application(int argc, char** argv, const std::string& title, WindowPosition position)
    : m_command_line_arguments{ argc, argv },
      m_window{ title, position },
      m_renderer{ m_window, m_command_line_arguments.target_fps.has_value() ? Renderer::VSync::Disabled
                                                                            : Renderer::VSync::Enabled },

      m_music_manager{ this, num_audio_channels },
      m_target_framerate{ m_command_line_arguments.target_fps },
      m_event_dispatcher{ &m_window } {
    initialize();
}

void Application::run() {
    m_event_dispatcher.register_listener(this);

#ifdef DEBUG_BUILD
    auto start_time = SDL_GetPerformanceCounter();
    const auto update_time = SDL_GetPerformanceFrequency() / 2; //0.5 s
    const double count_per_s = static_cast<double>(SDL_GetPerformanceFrequency());
    u64 frame_counter = 0;
#endif

    using std::chrono_literals::operator""s;
    const auto sleep_time = std::chrono::duration_cast<std::chrono::nanoseconds>(1s) / m_target_framerate.value();
    auto start_execution_time = std::chrono::steady_clock::now();

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

#ifdef DEBUG_BUILD
        ++frame_counter;

        const Uint64 current_time = SDL_GetPerformanceCounter();

        if (current_time - start_time >= update_time) {
            double elapsed = static_cast<double>(current_time - start_time) / count_per_s;
            m_fps_text->set_text(*this, fmt::format("FPS: {:.2f}", static_cast<double>(frame_counter) / elapsed));
            start_time = current_time;
            frame_counter = 0;
        }
#endif

        if (m_target_framerate.has_value()) {

            const auto now = std::chrono::steady_clock::now();
            const auto runtime = (now - start_execution_time);
            if (runtime < sleep_time) {
                //TODO: use SDL_DelayNS in sdl >= 3.0
                helper::sleep_nanoseconds(sleep_time - runtime);
                start_execution_time = std::chrono::steady_clock::now();
            } else {
                start_execution_time = now;
            }
        }
    }
}

void Application::handle_event(const SDL_Event& event, const Window* window) {
    if (event.type == SDL_QUIT) {
        m_is_running = false;
    }

    auto handled = false;

    for (const auto& scene : std::ranges::views::reverse(m_scene_stack)) {
        if (not handled and scene->handle_event(event, window)) {
            handled = true;
        }

        // detect if the scene overlaps everything, if that's the case, break out of the loop, since no other scene should receive inputs, since it's not visible to the user

        if (scene->get_layout().is_full_screen()) {
            break;
        }

        // if the scene is not covering the whole screen, it should give scenes in the background mouse events, but keyboard events are still only captured by the scene in focus, we also detect unhovers for whole scenes here
        if (utils::event_is_click_event(event, utils::CrossPlatformClickEvent::Any)) {
            if (not utils::is_event_in(window, event, scene->get_layout().get_rect())) {
                scene->on_unhover();
            }
        }
    }

    if (handled) {
        return;
    }

    // handle some special events

    switch (event.type) {
        case SDL_WINDOWEVENT:
            switch (event.window.event) {
                case SDL_WINDOWEVENT_HIDDEN:
                case SDL_WINDOWEVENT_MINIMIZED:
                case SDL_WINDOWEVENT_LEAVE: {
                    for (const auto& scene : m_scene_stack) {
                        scene->on_unhover();
                    }
                    break;
                }
                default:
                    break;
            }
            break;
        default:
            break;
    }

    // this global event handlers (atm only one) are special cases, they receive all inputs if they are not handled by the scenes explicably

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
                    helper::overloaded{
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
#ifdef DEBUG_BUILD
    m_fps_text->render(*this);
#endif
}

void Application::initialize() {
    try_load_settings();
    load_resources();
    push_scene(scenes::create_scene(*this, SceneId::MainMenu, ui::FullScreenLayout{ m_window }));

#ifdef DEBUG_BUILD
    m_fps_text = std::make_unique<Text>(
            this, "FPS: ?", fonts().get(FontId::Default), Color::white(),
            ui::RelativeLayout(window(), 0.01, 0.01, 0.1, 0.05).get_rect()
    );
#endif
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
    constexpr auto fonts_size = 128;
    const std::vector<std::tuple<FontId, std::string>> fonts{
        {       FontId::Default,   "PressStart2P.ttf"},
        {         FontId::Arial,          "arial.ttf"},
        {FontId::NotoColorEmoji, "NotoColorEmoji.ttf"},
        {       FontId::Symbola,        "Symbola.ttf"}
    };
    for (const auto& [font_id, path] : fonts) {
        const auto font_path = utils::get_assets_folder() / "fonts" / path;
        m_font_manager.load(font_id, font_path, fonts_size);
    }
}
