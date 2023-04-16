#include "tetris_application.hpp"
#include "local_multiplayer.hpp"
#include "play_mode.hpp"
#include <algorithm>

TetrisApplication::TetrisApplication(CommandLineArguments command_line_arguments)
#if defined(__ANDROID__)
    : Application{ "OOPetris", WindowPosition::Centered, std::move(command_line_arguments) } {
#else
    : Application{ "OOPetris", WindowPosition::Centered, width, height, std::move(command_line_arguments) } {
#endif
    try_load_settings();

    if (is_replay_mode()) {
        m_recording_reader = std::make_unique<RecordingReader>(*(this->command_line_arguments().recording_path));
    }

    const auto seed = create_seeds(1);

    const auto& [is_multiplayer, is_server, player_nums] = this->command_line_arguments().play_mode_information;
    if (is_multiplayer) {
        m_play_mode = std::make_unique<LocalMultiplayer>(is_replay_mode(), player_nums, is_server);
    } else {
        m_play_mode = std::make_unique<SinglePlayer>(is_replay_mode());
    }

    auto initial_play_mode_data = m_play_mode->init(m_settings, seed[0]);
    if (!initial_play_mode_data.has_value()) {
        std::cerr << "Error in initializing PlayMode: " << initial_play_mode_data.error() << "\n";
        std::exit(2);
    }

    auto start_state = initial_play_mode_data.value();
    const auto num_tetrions = start_state.num_players;
    const auto seed_to_use = start_state.seed;


    //TODO: atm each tetrion has the same seed with multiple players
    const auto seeds = std::vector<Random::Seed>(num_tetrions, seed_to_use);

    if (game_is_recorded()) {
        const auto seeds_span = std::span{ seeds.data(), seeds.size() };
        m_recording_writer = create_recording_writer(create_tetrion_headers(seeds_span));
    }


    for (u8 tetrion_index = 0; tetrion_index < num_tetrions; ++tetrion_index) {
        const auto starting_level = starting_level_for_tetrion(tetrion_index);
        spdlog::info("starting level for tetrion {}: {}", tetrion_index, starting_level);

        m_tetrions.push_back(std::make_unique<Tetrion>(
                tetrion_index, seeds.at(tetrion_index), starting_level, recording_writer_optional(), is_multiplayer
        ));

        auto on_event_callback = create_on_event_callback(tetrion_index);

        const auto tetrion_pointer = m_tetrions.back().get();
        if (is_replay_mode()) {
            m_inputs.push_back(
                    create_replay_input(tetrion_index, m_recording_reader.get(), tetrion_pointer, [](InputEvent) {})
            );
        } else {

            auto input = m_play_mode->get_input(
                    tetrion_index, tetrion_pointer, std::move(on_event_callback), &m_event_dispatcher
            );

            m_inputs.push_back(std::move(input));
        }
    }
    for (const auto& tetrion : m_tetrions) {
        tetrion->spawn_next_tetromino();
    }


    //TODO if the window is to small to handle num_players, we have to resize in some way ,
    //TODO: if it's to big it has to be resized into an appropiate width

    //TODO reintroduce
    /*     [[maybe_unused]] const std::size_t game_field_size =
            (GameManager::size_per_field * num_tetrions) + ((num_players - 1) * GameManager::space_between);

 */
    //TODO: resize(), but then game_managers have to updated as well, to repaint or not?


    //TODO assert that enough settings are here fro example 4 players! (could be more eventually!)
}


void TetrisApplication::update_inputs() {
    for (const auto& input : m_inputs) {
        input->update();
    }
}

void TetrisApplication::late_update_inputs() {
    for (const auto& input : m_inputs) {
        input->late_update();
    }
}

void TetrisApplication::update_tetrions() {
    for (const auto& tetrion : m_tetrions) {
        tetrion->update();
    }
}

void TetrisApplication::update() {
    update_inputs();
    update_tetrions();
    late_update_inputs();
}

void TetrisApplication::render() const {
    Application::render(); // call parent function to clear the screen
    for (const auto& tetrion : m_tetrions) {
        tetrion->render(*this);
    }
}

[[nodiscard]] std::unique_ptr<Input> TetrisApplication::create_input(
        Controls controls,
        Tetrion* associated_tetrion,
        Input::OnEventCallback on_event_callback
) {
    return utils::create_input(controls, associated_tetrion, std::move(on_event_callback), &m_event_dispatcher);
}

[[nodiscard]] std::unique_ptr<Input> TetrisApplication::create_replay_input(
        const u8 tetrion_index,
        RecordingReader* const recording_reader,
        Tetrion* const associated_tetrion,
        Input::OnEventCallback on_event_callback
) {
    return std::make_unique<ReplayInput>(
            associated_tetrion, tetrion_index, std::move(on_event_callback), recording_reader
    );
}

[[nodiscard]] Input::OnEventCallback TetrisApplication::create_on_event_callback(const int tetrion_index) {
    if (m_recording_writer) {
        return [tetrion_index, this](InputEvent event) {
            m_recording_writer->add_event(static_cast<u8>(tetrion_index), Application::simulation_step_index(), event);
        };
    } else {
        return Input::OnEventCallback{}; // empty std::function object
    }
}

void TetrisApplication::try_load_settings() try {
    std::ifstream settings_file{ settings_filename };
    m_settings = nlohmann::json::parse(settings_file);
    spdlog::info("settings loaded");
} catch (...) {
    spdlog::error("unable to load settings from \"{}\"", settings_filename);
    spdlog::warn("applying default settings");
}

[[nodiscard]] bool TetrisApplication::is_replay_mode() const {
    return this->command_line_arguments().recording_path.has_value();
}

[[nodiscard]] bool TetrisApplication::game_is_recorded() const {
    return not is_replay_mode();
}

[[nodiscard]] Random::Seed TetrisApplication::seed_for_tetrion(const u8 tetrion_index, const Random::Seed common_seed)
        const {
    return (is_replay_mode() ? m_recording_reader->tetrion_headers().at(tetrion_index).seed : common_seed);
}

[[nodiscard]] auto TetrisApplication::starting_level_for_tetrion(const u8 tetrion_index) const
        -> decltype(CommandLineArguments::starting_level) {
    return is_replay_mode() ? m_recording_reader->tetrion_headers().at(tetrion_index).starting_level
                            : this->command_line_arguments().starting_level;
}

[[nodiscard]] TetrisApplication::TetrionHeaders TetrisApplication::create_tetrion_headers(
        const std::span<const Random::Seed> seeds
) const {
    const auto num_tetrions = seeds.size();
    auto headers = TetrionHeaders{};
    headers.reserve(num_tetrions);
    for (u8 tetrion_index = 0; tetrion_index < num_tetrions; ++tetrion_index) {
        assert(tetrion_index < seeds.size());
        const auto tetrion_seed = seeds[tetrion_index];
        const auto starting_level = starting_level_for_tetrion(tetrion_index);
        headers.push_back(Recording::TetrionHeader{ .seed = tetrion_seed, .starting_level = starting_level });
    }
    return headers;
}

[[nodiscard]] std::unique_ptr<RecordingWriter> TetrisApplication::create_recording_writer(TetrionHeaders tetrion_headers
) {
#if defined(__ANDROID__)
    // ON android the path where the app runs is not writeable, so get a writebale path
    char* pref_path = SDL_GetPrefPath("coder2k", "oopetris");
    if (!pref_path) {
        throw std::runtime_error{ "Failed in getting the Pref Path on android!" };
    }
    const auto recording_directory_path = std::filesystem::path{ std::string{ pref_path } } / "recordings";
#else
    static constexpr auto recordings_directory = "recordings";
    const auto recording_directory_path = std::filesystem::path{ recordings_directory };
#endif
    if (not std::filesystem::exists(recording_directory_path)) {
        std::filesystem::create_directory(recording_directory_path);
    }
    const auto filename = fmt::format("{}.rec", utils::current_date_time_iso8601());
    const auto file_path = recording_directory_path / filename;

    return std::make_unique<RecordingWriter>(file_path, std::move(tetrion_headers));
}

[[nodiscard]] std::vector<Random::Seed> TetrisApplication::create_seeds(const u8 num_tetrions) const {
    auto seeds = std::vector<Random::Seed>{};
    seeds.reserve(num_tetrions);
    const auto common_seed = Random::generate_seed();
    for (u8 tetrion_index = 0; tetrion_index < num_tetrions; ++tetrion_index) {
        const auto seed = seed_for_tetrion(tetrion_index, common_seed);
        spdlog::info("seed for tetrion {}: {}", tetrion_index, seed);
        seeds.push_back(seed);
    }
    return seeds;
}

[[nodiscard]] tl::optional<RecordingWriter*> TetrisApplication::recording_writer_optional() {
    if (m_recording_writer) {
        return m_recording_writer.get();
    }
    return tl::nullopt;
}
