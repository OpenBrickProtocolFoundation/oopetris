
#pragma once

#include "helper/date.hpp"
#include "helper/optional.hpp"
#include "input/game_input.hpp"
#include "manager/service_provider.hpp"
#include "recordings/recording_writer.hpp"

#include <memory>

namespace tetrion {
    struct StartingParameters {
        u32 target_fps;
        Random::Seed seed;
        u32 starting_level;
        u8 tetrion_index;
        helper::optional<std::shared_ptr<recorder::RecordingWriter>> recording_writer;

        StartingParameters(
                u32 target_fps,
                Random::Seed seed,
                u32 starting_level, // NOLINT(bugprone-easily-swappable-parameters)
                u8 tetrion_index,
                helper::optional<std::shared_ptr<recorder::RecordingWriter>> recording_writer = helper::nullopt
        )
            : target_fps{ target_fps },
              seed{ seed },
              starting_level{ starting_level },
              tetrion_index{ tetrion_index },
              recording_writer{ std::move(recording_writer) } { }
    };
} // namespace tetrion

namespace input {

    using AdditionalInfo = std::tuple<std::shared_ptr<input::GameInput>, tetrion::StartingParameters>;

    [[nodiscard]] std::vector<input::AdditionalInfo>
    get_game_parameters_for_replay(ServiceProvider* service_provider, const std::filesystem::path& recording_path);

    [[nodiscard]] helper::optional<input::AdditionalInfo> get_single_player_game_parameters(
            ServiceProvider* service_provider,
            recorder::AdditionalInformation&& information,
            const date::ISO8601Date& date
    );

} // namespace input
