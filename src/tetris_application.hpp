#pragma once

#include "application.hpp"
#include "recording.hpp"
#include "settings.hpp"
#include "tetrion.hpp"
#include "tetromino_type.hpp"
#include <cassert>
#include <fstream>
#include <span>
#include <spdlog/spdlog.h>
#include <stdexcept>

/**
 * @brief This struct inherits from Application and implements the game's functionality. Instantiate this struct
 * and call run() on it to start the main game loop.
 */
struct TetrisApplication final : public Application {
private:
    using TetrionHeaders = std::vector<Recording::TetrionHeader>;

    static constexpr auto settings_filename = "settings.json";

    std::vector<std::unique_ptr<Tetrion>> m_tetrions;
    std::vector<std::unique_ptr<Input>> m_inputs;
    Settings m_settings;
    std::unique_ptr<RecordingWriter> m_recording_writer;
    std::unique_ptr<RecordingReader> m_recording_reader;

public:
    static constexpr int width = 800;
    static constexpr int height = 600;

    /**
     * @brief Constructs a TetrisApplication instance.
     * @param command_line_arguments The command line arguments passed to the executable.
     */
    explicit TetrisApplication(CommandLineArguments command_line_arguments);

protected:
    /**
     * @brief Calls the update() functions on all elements of m_inputs.
     */
    void update_inputs();
    void late_update_inputs();

    /**
     * @brief Calls the update() functions on all elements of m_tetrions.
     */
    void update_tetrions();

    /**
     * @brief Updates all inputs and all tetrions.
     */
    void update() override;

    /**
     * @brief Renders the contents of the window.
     */
    void render() const override;

private:
    /**
     * @brief Creates an input from given controls.
     * @param controls The controls (e.g. keyboard bindings) to apply for this control.
     * @param associated_tetrion The tetrion that is targeted by this input.
     * @param on_event_callback Callback that is invoked when an input event happens.
     * @return The input.
     */
    [[nodiscard]] std::unique_ptr<Input>
    create_input(Controls controls, Tetrion* associated_tetrion, Input::OnEventCallback on_event_callback);

    [[nodiscard]] static std::unique_ptr<Input> create_replay_input(
    /**
     * @brief Creates an input that replays a recorded game.
     * @param tetrion_index The index of the tetrion that is targeted by this input.
     * @param recording_reader A pointer to the RecordingReader that holds the recorded game.
     * @param associated_tetrion The tetrion that is targeted by this input.
     * @param on_event_callback Callback that is invoked when an input event happens.
     * @return The input.
     */
    [[nodiscard]] static std::unique_ptr<Input> create_recording_input(
            u8 tetrion_index,
            RecordingReader* constrecording_reader,
            Tetrion *constassociated_tetrion,
            Input::OnEventCallback on_event_callback
    );

    /**
     * @brief Creates a callback that can be passed to create_input() or create_recording_input(). If a game is being
     * recorded, the returned callback stores all events in the associated RecordingWriter (m_recording_writer).
     * Otherwise, a callback that does nothing is returned.
     * @param tetrion_index The index of the tetrion that shall be recorded.
     * @return The callback.
     */
    [[nodiscard]] Input::OnEventCallback create_on_event_callback(int tetrion_index);

    /**
     * @brief Tries to load the settings from settings.json and stores them into m_settings. Applies default
     * values upon failure.
     */
    void try_load_settings();

    /**
     * @brief Returns true if the path to a recorded game was specified as command line argument, false otherwise.
     * @return Whether a replay shall be replayed.
     */
    [[nodiscard]] bool is_replay_mode() const;

    /**
     * @brief Returns true if the current game shall be recorded, false otherwise. The game shall be recorded if it's
     * not replaying a recorded game.
     * @return Whether the game shall be recorded.
     */
    [[nodiscard]] bool game_is_recorded() const;

    /**
     * @brief Returns the random seed to be used for a given tetrion. The choice is based on whether a recorded
     * game is being replayed (then the seed of the recording is used) or not (then the passed common_seed is used).
     * @param tetrion_index The index of the tetrion this seed shall be used for.
     * @param common_seed The common random seed to be used if the application is not replaying a recorded game.
     * @return The seed for the tetrion.
     */
    [[nodiscard]] Random::Seed seed_for_tetrion(u8 tetrion_index, Random::Seed common_seed) const;

    /**
     * @brief Returns the starting level for a given tetrion. The choice is based on whether a recorded game is
     * being replayed (then the starting level of the recording is used) or not (then the starting level is taken
     * from the command line arguments (maybe its default value).
     * @param tetrion_index The index of the tetrion this starting level shall be used for.
     * @return The starting level.
     */
    [[nodiscard]] auto starting_level_for_tetrion(u8 tetrion_index) const
            -> decltype(CommandLineArguments::starting_level);

    /**
     * @brief Creates the TetrionHeader instances to be used when a game shall be recorded. This data is part of the
     * header of the recording.
     * @param seeds The seeds of all tetrions.
     * @return The tetrion headers for all tetrions.
     */
    [[nodiscard]] TetrionHeaders create_tetrion_headers(std::span<const Random::Seed> seeds) const;

    /**
     * @brief Creates the subdirectory for recorded games (if necessary) and instantiates a RecordingWriter to start
     * recording the game. The output filename is based on the current date and time.
     * @param tetrion_headers The tetrion headers of all tetrions.
     * @return The RecordingWriter.
     */
    [[nodiscard]] static std::unique_ptr<RecordingWriter> create_recording_writer(TetrionHeaders tetrion_headers);

    /**
     * @brief Creates seeds for a given number of tetrions.
     * @param num_tetrions The number of tetrions.
     * @return The seeds for all tetrions.
     */
    [[nodiscard]] std::vector<Random::Seed> create_seeds(u8 num_tetrions) const;

    /**
     * @brief Returns an tl::optional containing a pointer to the object managed by m_recording_writer if present.
     * Otherwise returns an empty tl::optional.
     * @return The pointer to the RecordingWriter as an tl::optional.
     */
    [[nodiscard]] tl::optional<RecordingWriter*> recording_writer_optional();
};
