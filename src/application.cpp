#include "application.hpp"
#include "helper/errors.hpp"
#include "helper/message_box.hpp"
#include "helper/sleep.hpp"
#include "platform/capabilities.hpp"
#include "scenes/scene.hpp"

#include <chrono>
#include <ranges>
#include <stdexcept>

#if defined(__CONSOLE__)
#include "helper/console_helpers.hpp"
#endif

namespace {

    [[nodiscard]] helper::MessageBox::Type get_notification_level(helper::error::Severity severity) {
        return severity == helper::error::Severity::Fatal   ? helper::MessageBox::Type::Error
               : severity == helper::error::Severity::Major ? helper::MessageBox::Type::Warning
                                                            : helper::MessageBox::Type::Information;
    }

} // namespace


Application::Application(std::unique_ptr<Window>&& window, const std::vector<std::string>& arguments) try
    : m_command_line_arguments{ arguments },
      m_window{ std::move(window) },
      m_renderer{ *m_window, m_command_line_arguments.target_fps.has_value() ? Renderer::VSync::Disabled
                                                                             : Renderer::VSync::Enabled },
      m_music_manager{ this, num_audio_channels },
      m_target_framerate{ m_command_line_arguments.target_fps },
      m_event_dispatcher{ m_window.get() } {
    initialize();
} catch (const helper::GeneralError& general_error) {
    const auto severity = general_error.severity();
    const auto notification_level = get_notification_level(severity);

    window->show_simple(notification_level, "Initialization Error", general_error.message());

    if (severity == helper::error::Severity::Fatal) {
        // rethrow the error, so that the caller gets an exception, since this error is fatal!
        throw general_error;
    }
}


void Application::run() {
    m_event_dispatcher.register_listener(this);

#ifdef DEBUG_BUILD
    auto start_time = SDL_GetPerformanceCounter();
    const auto update_time = SDL_GetPerformanceFrequency() / 2; //0.5 s
    const auto count_per_s = static_cast<double>(SDL_GetPerformanceFrequency());
    u64 frame_counter = 0;
#endif
    using namespace std::chrono_literals;

    const auto sleep_time = m_target_framerate.has_value() ? std::chrono::duration_cast<std::chrono::nanoseconds>(1s)
                                                                     / m_target_framerate.value()
                                                           : 0s;
    auto start_execution_time = std::chrono::steady_clock::now();

    while (m_is_running

#if defined(__CONSOLE__)
           and console::inMainLoop()
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
        try {
            auto [scene_update, scene_change] = m_scene_stack.at(index)->update();

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
                                    m_scene_stack.push_back(scenes::create_scene(*this, push.target_scene, push.layout)
                                    );
                                },
                                [this](scenes::Scene::RawPush& raw_push) {
                                    spdlog::info("pushing back scene {}", raw_push.name);
                                    m_scene_stack.push_back(std::move(raw_push.scene));
                                },
                                [this](const scenes::Scene::Switch& switch_) {
                                    spdlog::info("switching to scene {}", magic_enum::enum_name(switch_.target_scene));
                                    auto scene = scenes::create_scene(*this, switch_.target_scene, switch_.layout);

                                    // only clear, after the construction was successful
                                    m_scene_stack.clear();
                                    m_scene_stack.push_back(std::move(scene));
                                },
                                [this](scenes::Scene::RawSwitch& raw_switch) {
                                    spdlog::info("switching to scene {}", raw_switch.name);
                                    m_scene_stack.clear();
                                    m_scene_stack.push_back(std::move(raw_switch.scene));
                                },
                                [this](const scenes::Scene::Exit&) { m_is_running = false; },
                        },
                        *scene_change
                );
            }
            if (scene_update == scenes::SceneUpdate::StopUpdating) {
                break;
            }

            // if an error occurred on:
            // - creation:  the creation wasn't finished, so just not pushing / switching to the scene
            // -update: the update failed in the middle, and the scene, that caused the error, has to make sure, that ignoring it, (and not crashing) resets the state, so that this doesn't occur on every frame

        } catch (const std::runtime_error& error) {
            m_window->show_simple(helper::MessageBox::Type::Error, "Error on Scene Initialization", error.what());
        } catch (const helper::GeneralError& general_error) {
            const auto notification_level = get_notification_level(general_error.severity());

            m_window->show_simple(notification_level, "Error on Scene Initialization", general_error.message());
        }
    }

#if defined(_HAVE_DISCORD_SDK)

    if (m_discord_instance.has_value()) {
        m_discord_instance->update();
    }

#endif
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
    push_scene(scenes::create_scene(*this, SceneId::MainMenu, ui::FullScreenLayout{ *m_window }));

#ifdef DEBUG_BUILD
    m_fps_text = std::make_unique<ui::Label>(
            this, "FPS: ?", fonts().get(FontId::Default), Color::white(), std::pair<double, double>{ 0.95, 0.95 },
            ui::Alignment{ ui::AlignmentHorizontal::Middle, ui::AlignmentVertical::Center },
            ui::RelativeLayout{ window(), 0.0, 0.0, 0.1, 0.05 }, false
    );
#endif

#if defined(_HAVE_DISCORD_SDK)
    if (m_settings.discord) {
        auto discord_instance = DiscordInstance::initialize();
        if (not discord_instance.has_value()) {
            spdlog::warn(
                    "Error initializing the discord instance, it might not be running: {}", discord_instance.error()
            );
        } else {
            m_discord_instance = std::move(discord_instance.value());
            m_discord_instance->after_setup();
        }
    }

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

    // apply settings
    m_music_manager.set_volume(m_settings.volume);
}

void Application::load_resources() {
    constexpr auto fonts_size = 128;
    const std::vector<std::tuple<FontId, std::string>> fonts{
#if defined(__3DS__)
        //TODO: debug why the other font crashed, not on loading, but on trying to render text!
        {        FontId::Default, "LeroyLetteringLightBeta01.ttf" },
#else
        { FontId::Default, "PressStart2P.ttf" },
#endif
        {          FontId::Arial,                     "arial.ttf" },
        { FontId::NotoColorEmoji,            "NotoColorEmoji.ttf" },
        {        FontId::Symbola,                   "Symbola.ttf" }
    };
    for (const auto& [font_id, path] : fonts) {
        const auto font_path = utils::get_assets_folder() / "fonts" / path;
        m_font_manager.load(font_id, font_path, fonts_size);
    }
}

#if defined(_HAVE_DISCORD_SDK)

[[nodiscard]] helper::optional<DiscordInstance>& Application::discord_instance() {
    return m_discord_instance;
}

[[nodiscard]] const helper::optional<DiscordInstance>& Application::discord_instance() const {
    return m_discord_instance;
}


#endif
