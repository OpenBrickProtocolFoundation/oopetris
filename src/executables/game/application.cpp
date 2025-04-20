#include <core/helper/errors.hpp>
#include <core/helper/magic_enum_wrapper.hpp>
#include <core/helper/sleep.hpp>

#include "application.hpp"
#include "helper/graphic_utils.hpp"
#include "helper/message_box.hpp"
#include "input/input.hpp"
#include "manager/music_manager.hpp"
#include "scenes/scene.hpp"
#include "ui/layout.hpp"


#include <fmt/chrono.h>
#include <memory>
#include <ranges>
#include <stdexcept>

#if defined(__CONSOLE__)
#include "helper/console_helpers.hpp"
#endif

#if !defined(NDEBUG)
#include "graphics/text.hpp"
#endif


#if defined(__EMSCRIPTEN__)
#include <emscripten.h>
#endif

namespace {

    [[nodiscard]] helper::MessageBox::Type get_notification_level(helper::error::Severity severity) {
        return severity == helper::error::Severity::Fatal   ? helper::MessageBox::Type::Error
               : severity == helper::error::Severity::Major ? helper::MessageBox::Type::Warning
                                                            : helper::MessageBox::Type::Information;
    }

} // namespace


#if !defined(NDEBUG)
helper::DebugInfo::DebugInfo(Uint64 start_time, u64 frame_counter, Uint64 update_time, double count_per_s)
    : m_start_time{ start_time },
      m_frame_counter{ frame_counter },
      m_update_time{ update_time },
      m_count_per_s{ count_per_s } { }

[[nodiscard]] Uint64 helper::DebugInfo::update_time() const {
    return m_update_time;
}

[[nodiscard]] double helper::DebugInfo::count_per_s() const {
    return m_count_per_s;
}
#endif

helper::TimeInfo::TimeInfo(
        std::chrono::nanoseconds sleep_time,
        std::chrono::steady_clock::time_point start_execution_time
)
    : m_sleep_time{ sleep_time },
      m_start_execution_time{ start_execution_time } { }

[[nodiscard]] std::chrono::nanoseconds helper::TimeInfo::sleep_time() const {
    return m_sleep_time;
}

helper::LoadingInfo::LoadingInfo(
        std::chrono::nanoseconds sleep_time,
        Uint64 start_time,
        std::future<void>&& load_everything_thread,
        std::chrono::steady_clock::time_point start_execution_time,
        bool finished_loading,
        scenes::LoadingScreen&& loading_screen
)
    : m_sleep_time{ sleep_time },
      m_start_time{ start_time },
      m_load_everything_thread{ std::move(load_everything_thread) },
      m_start_execution_time{ start_execution_time },
      m_finished_loading{ finished_loading },
      m_loading_screen{ std::move(loading_screen) } { }

[[nodiscard]] std::chrono::nanoseconds helper::LoadingInfo::sleep_time() const {
    return m_sleep_time;
}

[[nodiscard]] Uint64 helper::LoadingInfo::start_time() const {
    return m_start_time;
}


[[nodiscard]] const std::future<void>& helper::LoadingInfo::load_everything_thread() const {
    return m_load_everything_thread;
}

Application::Application(std::shared_ptr<Window>&& window, CommandLineArguments&& arguments) try
    : m_command_line_arguments{ std::move(arguments) },
      m_window{ std::move(window) },
      m_renderer{ *m_window, m_command_line_arguments.target_fps.has_value() ? Renderer::VSync::Disabled
                                                                             : Renderer::VSync::Enabled },
      m_target_framerate{ m_command_line_arguments.target_fps }

#if defined(__EMSCRIPTEN__)
      ,
      m_web_context{ this }
#endif
{
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

Application::~Application() = default;

#if defined(__EMSCRIPTEN__)
void c_loop_entry(void* arg) {
    auto application = reinterpret_cast<Application*>(arg);
    application->emscripten_do_process();
    application->loop_entry_emscripten();
}

void Application::load_emscripten() {

    if ((not m_loading_info->m_finished_loading) and m_is_running) {
        load_loop();
        return;
    }

    const auto duration = std::chrono::milliseconds(SDL_GetTicks64() - m_loading_info->start_time());

    // we can reach this via SDL_QUIT or SDL_APP_TERMINATING
    if (not m_loading_info->m_finished_loading or not m_is_running) {

        spdlog::debug("Aborted loading after {}", duration);

        // do some combination of the loading exit in a normal OS case and the emscripten normal game loop exit
        this->~Application();
        emscripten_cancel_main_loop();
        utils::exit(0);
    }


    spdlog::debug("Took {} to load", duration);

    push_scene(scenes::create_scene(*this, SceneId::MainMenu, ui::FullScreenLayout{ *m_window }));

    // run this manually, in a normal case, this would be run after the loader has finished
    this->run();
}

void Application::main_loop_emscripten() {
    if (not this->m_is_running) {
        // call the destructor manually, so that everything gets cleaned up
        this->~Application();
        emscripten_cancel_main_loop();
        return;
    }

    main_loop();
}
void Application::emscripten_do_process() {
    m_web_context.do_processing();
}

#endif

void Application::run() {
    m_event_dispatcher.register_listener(this);

#if !defined(NDEBUG)
    auto start_time = SDL_GetPerformanceCounter();

    const auto update_time = SDL_GetPerformanceFrequency() / 2; //0.5 s

    const auto count_per_s = static_cast<double>(SDL_GetPerformanceFrequency());

    u64 frame_counter = 0;
    m_debug = std::make_unique<helper::DebugInfo>(start_time, frame_counter, update_time, count_per_s);
#endif
    using namespace std::chrono_literals;

    const auto sleep_time = m_target_framerate.has_value() ? std::chrono::duration_cast<std::chrono::nanoseconds>(1s)
                                                                     / m_target_framerate.value()
                                                           : 0s;
    auto start_execution_time = std::chrono::steady_clock::now();

    m_time_info = std::make_unique<helper::TimeInfo>(sleep_time, start_execution_time);

#if defined(__EMSCRIPTEN__)
    m_current_emscripten_func = std::bind(&Application::main_loop_emscripten, this);
    return;
#else

    while (m_is_running

#if defined(__CONSOLE__)
           and console::inMainLoop()
#endif

    ) {
        main_loop();
    }
#endif
}

void Application::main_loop() {

    m_event_dispatcher.dispatch_pending_events();
    update();
    render();
    m_renderer.present();

#if !defined(NDEBUG)
    m_debug->m_frame_counter++;

    const Uint64 current_time = SDL_GetPerformanceCounter();

    if (current_time - m_debug->m_start_time >= m_debug->update_time()) {
        const double elapsed = static_cast<double>(current_time - m_debug->m_start_time) / m_debug->count_per_s();

        m_fps_text->set_text(
                *this, fmt::format("FPS: {:.2f}", static_cast<double>(m_debug->m_frame_counter) / elapsed)
        );

        m_debug->m_start_time = current_time;
        m_debug->m_frame_counter = 0;
    }
#endif

    if (m_target_framerate.has_value()) {

        const auto now = std::chrono::steady_clock::now();
        const auto runtime = (now - m_time_info->m_start_execution_time);

        const auto sleep_time = m_time_info->sleep_time();

        if (runtime < sleep_time) {
            //TODO(totto): use SDL_DelayNS in sdl >= 3.0
            helper::sleep_nanoseconds(sleep_time - runtime);
            m_time_info->m_start_execution_time = std::chrono::steady_clock::now();
        } else {
            m_time_info->m_start_execution_time = now;
        }
    }
}

void Application::load_loop() {

    // we can't use the normal event loop, so we have to do it manually
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
        if (event.type == SDL_QUIT) {
            m_is_running = false;
        }

        // special event for android and IOS
        if (event.type == SDL_APP_TERMINATING) {
            m_is_running = false;
        }
    }

    if (not m_is_running) {
        return;
    }

    m_loading_info->m_loading_screen.update();
    // this service_provider only guarantees the renderer + the window to be accessible without race conditions
    m_loading_info->m_loading_screen.render(*this);

    // present and  wait (depending if vsync is on or not, this has to be done manually)
    m_renderer.present();

    if (m_target_framerate.has_value()) {

        const auto now = std::chrono::steady_clock::now();
        const auto runtime = (now - m_loading_info->m_start_execution_time);

        const auto sleep_time = m_loading_info->sleep_time();

        if (runtime < sleep_time) {
            //TODO(totto): use SDL_DelayNS in sdl >= 3.0
            helper::sleep_nanoseconds(sleep_time - runtime);
            m_loading_info->m_start_execution_time = std::chrono::steady_clock::now();
        } else {
            m_loading_info->m_start_execution_time = now;
        }
    }
    // end waiting

    // wait until is faster, since it just compares two time_points instead of getting now() and than adding the wait-for argument
    m_loading_info->m_finished_loading =
            m_loading_info->load_everything_thread().wait_until(std::chrono::system_clock::time_point::min())
            == std::future_status::ready;
}


void Application::handle_event(const SDL_Event& event) {
    if (event.type == SDL_QUIT) {
        m_is_running = false;
    }

    // special event for android and IOS
    if (event.type == SDL_APP_TERMINATING) {
        m_is_running = false;
    }

    auto handled = false;

    for (const auto& scene : std::ranges::views::reverse(m_scene_stack)) {
        if (not handled and scene->handle_event(m_input_manager, event)) {
            handled = true;
        }

        // detect if the scene overlaps everything, if that's the case, break out of the loop, since no other scene should receive inputs, since it's not visible to the user

        if (scene->get_layout().is_full_screen()) {
            break;
        }

        // if the scene is not covering the whole screen, it should give scenes in the background mouse events, but keyboard events are still only captured by the scene in focus, we also detect unhovers for whole scenes here
        if (const auto result = m_input_manager->get_pointer_event(event); result.has_value()) {
            if (not result->is_in(scene->get_layout().get_rect())) {
                scene->on_unhover();
            }
        }
    }

    if (handled) {
        return;
    }

    // handle some special events
    const auto is_special_event = m_input_manager->process_special_inputs(event);
    if (is_special_event) {

        if (const auto special_event = is_special_event.get_additional(); special_event.has_value()) {
            if (special_event.value() == input::SpecialRequest::WindowFocusLost) {
                for (const auto& scene : m_scene_stack) {
                    scene->on_unhover();
                }
            }
        }
    }


    // this global event handlers (atm only one) are special cases, they receive all inputs if they are not handled by the scenes explicitly

    if (m_music_manager->handle_event(m_input_manager, event)) {
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
                        helper::Overloaded{
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
                                [this](const scenes::Scene::Switch& scene_switch) {
                                    spdlog::info(
                                            "switching to scene {}", magic_enum::enum_name(scene_switch.m_target_scene)
                                    );
                                    auto scene = scenes::create_scene(
                                            *this, scene_switch.m_target_scene, scene_switch.m_layout
                                    );

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

#if defined(_HAVE_DISCORD_SOCIAL_SDK)

    if (m_discord_instance.has_value()) {

        switch (m_discord_instance->get_status()) {
            case DiscordStatus::Starting:
                break;
            case DiscordStatus::Error:
                m_discord_instance = std::nullopt;
                spdlog::warn("Error initializing the discord instance, it might not be running, destroying client!");
                break;
            case DiscordStatus::Ok:
                m_discord_instance->update();
                break;
        }
    }

#endif
}

void Application::render() const {
    renderer().clear();
    for (const auto& scene : m_scene_stack) {
        scene->render(*this);
    }
#if !defined(NDEBUG)
    m_fps_text->render(*this);
#endif
}

#if defined(__EMSCRIPTEN__)
void Application::loop_entry_emscripten() {
    this->m_current_emscripten_func();
}

#endif


void Application::initialize() {

    auto loading_screen_arg = scenes::LoadingScreen{ this };

    const auto start_time = SDL_GetTicks64();

    std::future<void> load_everything_thread = std::async(std::launch::async, [this] {
        this->m_settings_manager = std::make_unique<SettingsManager>(this);

        this->m_settings_manager->add_callback([this](const auto& settings) { this->reload_api(settings); });

        const auto current_settings = this->m_settings_manager->settings();

        this->m_music_manager = std::make_unique<MusicManager>(this, num_audio_channels);
        this->m_music_manager->set_volume(current_settings.volume, true, true);

        this->m_input_manager = std::make_shared<input::InputManager>(this->m_window);

        this->m_font_manager = std::make_unique<FontManager>();

        this->reload_api(current_settings);

        this->load_resources();

#if !defined(NDEBUG)
        //TODO(Totto): emscripten: this is using sdl rendering (to a texture) in another thread then the main thread, use proxying to the main thread here too, and disable OOPETRIS_DONT_USE_PRERENDERED_TEXT
        m_fps_text = std::make_unique<ui::Label>(
                this, "FPS: ?", font_manager().get(FontId::Default), Color::white(),
                std::pair<double, double>{ 0.95, 0.95 },
                ui::Alignment{ ui::AlignmentHorizontal::Middle, ui::AlignmentVertical::Center },
                ui::RelativeLayout{ window(), 0.0, 0.0, 0.1, 0.05 }, false
        );
#endif

#if defined(_HAVE_DISCORD_SOCIAL_SDK)
        if (current_settings.discord) {
            m_discord_instance = DiscordInstance{};
        }

#endif
    });


    using namespace std::chrono_literals;

    const auto sleep_time = m_target_framerate.has_value() ? std::chrono::duration_cast<std::chrono::nanoseconds>(1s)
                                                                     / m_target_framerate.value()
                                                           : 0s;
    auto start_execution_time_arg = std::chrono::steady_clock::now();

    m_loading_info = std::make_unique<helper::LoadingInfo>(
            sleep_time, start_time, std::move(load_everything_thread), start_execution_time_arg, false,
            std::move(loading_screen_arg)
    );

    // this is a duplicate of below in some cases, but it's just for the loading screen and can't be factored out easily
    // this also only uses a subset of all things, the real event loop uses, so that nothing breaks while doing multithreading
    // the only things usable are: (since NOT accessed (writing) via the loading thread and already initialized):
    // - m_command_line_arguments
    // - m_window
    // - m_renderer
    // - m_target_framerate

#if defined(__EMSCRIPTEN__)
    m_current_emscripten_func = std::bind(&Application::load_emscripten, this);
    int selected_fps = m_target_framerate.has_value() ? m_target_framerate.value() : -1;

    // NOTE: this is complicated, especially in C++
    // see: https://wiki.libsdl.org/SDL2/README/emscripten#porting-your-app-to-emscripten
    // and: https://emscripten.org/docs/api_reference/emscripten.h.html#c.emscripten_set_main_loop_arg
    // for a basic understanding
    // this sets up a loop,, throws an exception(a special kind, not c++ one) to exit this function, but nothing gets cleaned up (no destructors get called, this function NEVER returns)
    // but after emscripten_cancel_main_loop we have to manually call the destructor, to clean up,
    emscripten_set_main_loop_arg(c_loop_entry, this, selected_fps, true);
    UNREACHABLE();
#else
    while ((not m_loading_info->m_finished_loading) and m_is_running
#if defined(__CONSOLE__)
           and console::inMainLoop()
#endif
    ) {
        load_loop();
    }


    const auto duration = std::chrono::milliseconds(SDL_GetTicks64() - start_time);

    // we can reach this via SDL_QUIT, SDL_APP_TERMINATING or (not console::inMainLoop())
    if (not m_loading_info->m_finished_loading or not m_is_running) {

        spdlog::debug("Aborted loading after {}", duration);

        // just exit immediately, without cleaning up, since than we would have to cancel the loading thread somehow, which is way to complicated, let the OS clean up our mess we created here xD

        utils::exit(0);
    }


    spdlog::debug("Took {} to load", duration);

    push_scene(scenes::create_scene(*this, SceneId::MainMenu, ui::FullScreenLayout{ *m_window }));
#endif
}

void Application::load_resources() {
    constexpr auto fonts_size = 128;
    const std::vector<std::tuple<FontId, std::string>> fonts{
#if defined(__3DS__)
        //TODO(Totto): debug why the other font crashed, not on loading, but on trying to render text!
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
        m_font_manager->load(font_id, font_path, fonts_size);
    }
}

#if defined(_HAVE_DISCORD_SOCIAL_SDK)

[[nodiscard]] std::optional<DiscordInstance>& Application::discord_instance() {
    return m_discord_instance;
}

[[nodiscard]] const std::optional<DiscordInstance>& Application::discord_instance() const {
    return m_discord_instance;
}

#endif

#if defined(__EMSCRIPTEN__)

[[nodiscard]] web::WebContext& Application::web_context() {
    return m_web_context;
}

[[nodiscard]] const web::WebContext& Application::web_context() const {
    return m_web_context;
}

#endif


void Application::reload_api(const settings::Settings& settings) {

    if (auto api_url = settings.api_url; api_url.has_value()) {
        auto maybe_api = lobby::API::get_api(this, api_url.value());
        if (maybe_api.has_value()) {
            //TODO(Totto): do this somehow asynchronous
            m_api = std::make_unique<lobby::API>(std::move(maybe_api.value()));
        } else {
            spdlog::error("Error in connecting to lobby API: {}", maybe_api.error());
        }
    } else {
        spdlog::info("No lobby API provided");
    }
}


void Application::push_scene(std::unique_ptr<scenes::Scene> scene) {
    m_scene_stack.push_back(std::move(scene));
}

// implementation of ServiceProvider
[[nodiscard]] EventDispatcher& Application::event_dispatcher() {
    return m_event_dispatcher;
}

[[nodiscard]] const EventDispatcher& Application::event_dispatcher() const {
    return m_event_dispatcher;
}

FontManager& Application::font_manager() {
    return *m_font_manager;
}

[[nodiscard]] const FontManager& Application::font_manager() const {
    return *m_font_manager;
}

CommandLineArguments& Application::command_line_arguments() {
    return m_command_line_arguments;
}

[[nodiscard]] const CommandLineArguments& Application::command_line_arguments() const {
    return m_command_line_arguments;
}

SettingsManager& Application::settings_manager() {
    return *m_settings_manager;
}

[[nodiscard]] const SettingsManager& Application::settings_manager() const {
    return *m_settings_manager;
}

MusicManager& Application::music_manager() {
    return *m_music_manager;
}

[[nodiscard]] const MusicManager& Application::music_manager() const {
    return *m_music_manager;
}

[[nodiscard]] const Renderer& Application::renderer() const {
    return m_renderer;
}

[[nodiscard]] const Window& Application::window() const {
    return *m_window;
}

[[nodiscard]] Window& Application::window() {
    return *m_window;
}

[[nodiscard]] input::InputManager& Application::input_manager() {
    return *m_input_manager;
}

[[nodiscard]] const input::InputManager& Application::input_manager() const {
    return *m_input_manager;
}

[[nodiscard]] const std::unique_ptr<lobby::API>& Application::api() const {
    return m_api;
}
