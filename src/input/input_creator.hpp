
#pragma once

#include <core/helper/date.hpp>
#include <recordings/utility/recording_writer.hpp>

#include "helper/export_symbols.hpp"
#include "input/game_input.hpp"
#include "manager/service_provider.hpp"

#include <memory>

namespace tetrion {
    struct StartingParameters {
        u32 target_fps;
        Random::Seed seed;
        u32 starting_level;
        u8 tetrion_index;
        std::optional<std::shared_ptr<recorder::RecordingWriter>> recording_writer;

        StartingParameters(
                u32 target_fps_a, // NOLINT(bugprone-easily-swappable-parameters)
                Random::Seed seed_a,
                u32 starting_level_a,
                u8 tetrion_index_a,
                std::optional<std::shared_ptr<recorder::RecordingWriter>> recording_writer_a = std::nullopt
        )
            : target_fps{ target_fps_a },
              seed{ seed_a },
              starting_level{ starting_level_a },
              tetrion_index{ tetrion_index_a },
              recording_writer{ std::move(recording_writer_a) } { }
    };
} // namespace tetrion

namespace input {

    using AdditionalInfo = std::tuple<std::shared_ptr<input::GameInput>, tetrion::StartingParameters>;

    [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED
            std::pair<std::vector<input::AdditionalInfo>, recorder::AdditionalInformation>
            get_game_parameters_for_replay(
                    ServiceProvider* service_provider,
                    const std::filesystem::path& recording_path
            );

    [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED helper::expected<input::AdditionalInfo, std::string>
    get_single_player_game_parameters(
            ServiceProvider* service_provider,
            recorder::AdditionalInformation&& information,
            const date::ISO8601Date& date
    );

} // namespace input
