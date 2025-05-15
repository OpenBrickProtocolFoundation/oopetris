

#include <core/helper/date.hpp>
#include <core/helper/errors.hpp>
#include <core/helper/expected.hpp>
#include <recordings/utility/additional_information.hpp>

#include "game/command_line_arguments.hpp"
#include "helper/constants.hpp"
#include "helper/graphic_utils.hpp"
#include "input.hpp"
#include "input/replay_input.hpp"
#include "input_creator.hpp"

#include <fmt/format.h>
#include <core/helper/spdlog_wrapper.hpp>
#include <stdexcept>

namespace {


    [[nodiscard]] recorder::TetrionHeader create_tetrion_headers_for_one(const input::AdditionalInfo& info) {
        const auto& needed_info = std::get<1>(info);
        return recorder::TetrionHeader{ needed_info.seed, needed_info.starting_level };
    }

    [[nodiscard]] u32 get_target_fps(ServiceProvider* const service_provider) {
        u32 target_fps = 60;

        if (const auto optional_target_fps = service_provider->command_line_arguments().target_fps;
            optional_target_fps.has_value()) {
            target_fps = optional_target_fps.value();
        } else {
            SDL_DisplayMode mode{};
            const int mode_result = SDL_GetCurrentDisplayMode(0, &mode);
            if (mode_result != 0) {
                throw helper::FatalError{ fmt::format("failed in getting display mode: {}", SDL_GetError()) };
            }
            if (mode.refresh_rate != 0) {
                target_fps = static_cast<u32>(mode.refresh_rate);
            }
        }

        return target_fps;
    }


} // namespace


[[nodiscard]] std::pair<std::vector<input::AdditionalInfo>, recorder::AdditionalInformation>
input::get_game_parameters_for_replay(
        ServiceProvider* const service_provider,
        const std::filesystem::path& recording_path
) {

    std::vector<input::AdditionalInfo> result{};

    const auto target_fps = get_target_fps(service_provider);

    auto maybe_recording_reader = recorder::RecordingReader::from_path(recording_path);

    if (not maybe_recording_reader.has_value()) {
        throw std::runtime_error(
                fmt::format("an error occurred while reading recording: {}", maybe_recording_reader.error())
        );
    }

    const auto recording_reader =
            std::make_shared<recorder::RecordingReader>(std::move(maybe_recording_reader.value()));


    const auto tetrion_headers = recording_reader->tetrion_headers();


    result.reserve(tetrion_headers.size());

    for (u8 tetrion_index = 0; tetrion_index < static_cast<u8>(tetrion_headers.size()); ++tetrion_index) {

        const auto* primary_input = service_provider->input_manager().get_primary_input();

        auto input = std::make_unique<ReplayGameInput>(recording_reader, primary_input);

        const auto& header = tetrion_headers.at(tetrion_index);

        const auto seed = header.seed;
        const auto starting_level = header.starting_level;

        const tetrion::StartingParameters starting_parameters = { target_fps, seed, starting_level, tetrion_index,
                                                                  std::nullopt };

        result.emplace_back(std::move(input), starting_parameters);
    }


    return { result, recording_reader->information() };
}


[[nodiscard]] helper::expected<input::AdditionalInfo, std::string> input::get_single_player_game_parameters(
        ServiceProvider* const service_provider,
        recorder::AdditionalInformation&& information,
        const date::ISO8601Date& date
) {

    auto input = service_provider->input_manager().get_game_input(service_provider);
    if (not input.has_value()) {
        return helper::unexpected<std::string>{ input.error() };
    }

    const auto starting_level = service_provider->command_line_arguments().starting_level;

    const auto seed = Random::generate_seed();

    const auto target_fps = get_target_fps(service_provider);

    const tetrion::StartingParameters starting_parameters = { target_fps, seed, starting_level, 0 };

    AdditionalInfo result{ input.value(), starting_parameters };


    auto tetrion_header = create_tetrion_headers_for_one(result);
    std::vector<recorder::TetrionHeader> tetrion_headers{ tetrion_header };

    const auto recording_directory_path = utils::get_root_folder() / constants::recordings_directory;


    auto dir_result = utils::create_directory(recording_directory_path, true);
    if (not dir_result.has_value()) {

        const auto date_time_str = date.to_string();

        if (not date_time_str.has_value()) {
            throw std::runtime_error{ fmt::format("Erro in date to string conversion: {}", date_time_str.error()) };
        }

        const auto filename = fmt::format("{}.{}", date_time_str.value(), constants::recording::extension);
        const auto file_path = recording_directory_path / filename;


        auto recording_writer_create_result =
                recorder::RecordingWriter::get_writer(file_path, std::move(tetrion_headers), std::move(information));
        if (not recording_writer_create_result.has_value()) {
            throw std::runtime_error(recording_writer_create_result.error());
        }

        const auto recording_writer =
                std::make_shared<recorder::RecordingWriter>(std::move(recording_writer_create_result.value()));


        std::get<1>(result).recording_writer = recording_writer;
    } else {
        spdlog::warn("Couldn't create recordings folder {}: skipping creation of a recording", dir_result.value());
    }

    return result;
}
