#pragma once

#include "command_line_arguments.hpp"
#include "event_dispatcher.hpp"
#include "event_listener.hpp"
#include "renderer.hpp"
#include "sdl_context.hpp"
#include "types.hpp"
#include "window.hpp"

/**
 * @brief Abstract base for an application. Inherit from this struct to create an application and customize the
 * behavior. Do not create multiple instances of this struct.
 */
struct Application : public EventListener {
private:
    SdlContext context;
    Window m_window;
    Renderer m_renderer;
    bool m_is_running{ true };
    static inline u64 s_num_steps_simulated{ 0 };
    CommandLineArguments m_command_line_arguments;

protected:
    EventDispatcher m_event_dispatcher;

protected:
    /**
     * @brief Creates a new application.
     * @param title The text in the window title bar.
     * @param position The position where the window should spawn at.
     * @param width The width of the window.
     * @param height The height of the window.
     * @param command_line_arguments The command line arguments.
     */
    Application(
            const std::string& title,
            WindowPosition position,
            int width,
            int height,
            CommandLineArguments command_line_arguments
    );

    Application(const std::string& title, WindowPosition position, CommandLineArguments command_line_arguments);

public:
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    /**
     * @brief Starts the main game loop.
     */
    void run();

    /**
     * @brief Obtains the renderer for this application's window.
     * @return The renderer.
     */
    [[nodiscard]] const Renderer& renderer() const {
        return m_renderer;
    }

    /**
     * @brief Obtains the window for this application.
     * @return The window.
     */
    [[nodiscard]] const Window& window() const {
        return m_window;
    }

    /**
     * @brief Returns the elapsed time (in seconds) since the SDL2 library has been initialized.
     * @return The elapsed time in seconds.
     */
    static double elapsed_time() {
        return static_cast<double>(SDL_GetTicks()) / 1000.0;
    }

    /**
     * @brief Returns the index of the current simulation step. The application tries to execute a certain number
     * of simulation steps per second. This is specified in the command line arguments (passed to the constructor).
     * @return The index of the current simulation step.
     */
    static u64 simulation_step_index() {
        return s_num_steps_simulated;
    }

    /**
     * @brief Implements the EventListener. If the escape key is pressed or the windows is closed, the application
     * gets closed.
     * @param event The SDL event to handle.
     */
    void handle_event(const SDL_Event& event) override;

protected:
    /**
     * @brief This function gets called from within the main game loop (i.e. by the run() function). Override this
     * function to specify what should be done in every simulation step.
     */
    virtual void update() = 0;

    /**
     * @brief Clears the contents of the window with a black color. Override this function to render additional
     * window contents.
     */
    virtual void render() const;

    /**
     * @brief Returns the CommandLineArguments instance passed to this struct's constructor.
     * @return The command line arguments.
     */
    [[nodiscard]] const CommandLineArguments& command_line_arguments() const {
        return m_command_line_arguments;
    }
};
