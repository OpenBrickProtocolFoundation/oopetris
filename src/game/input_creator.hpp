
#pragma once

#include "helper/optional.hpp"
#include "manager/recording/recording_writer.hpp"
#include "manager/service_provider.hpp"
#include "platform/input.hpp"

#include <memory>
#include <vector>

namespace tetrion {
    struct StartingParameters {
        u32 target_fps;
        Random::Seed seed;
        u32 starting_level;
        u8 tetrion_index;
        helper::optional<std::shared_ptr<recorder::RecordingWriter>> recording_writer;
    };
} // namespace tetrion

namespace input {

    using AdditionalInfo = std::tuple<std::unique_ptr<Input>, tetrion::StartingParameters>;

    [[nodiscard]] std::vector<AdditionalInfo> get_game_parameters(
            ServiceProvider* service_provider,
            u32 amount,
            const std::vector<recorder::AdditionalInformation>& additional_info =
                    std::vector<recorder::AdditionalInformation>{}
    );

} // namespace input
