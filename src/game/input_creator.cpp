

#include "input_creator.hpp"
#include "helper/command_line_arguments.hpp"
#include "helper/date.hpp"
#include "platform/replay_input.hpp"
#include <stdexcept>

#if defined(__ANDROID__)
#include "platform/android_input.hpp"
#elif defined(__SWITCH__)
#include "platform/switch_input.hpp"
#else
#include "platform/keyboard_input.hpp"
#endif

#include <fmt/format.h>

namespace {

    [[nodiscard]] std::unique_ptr<Input> create_input(ServiceProvider* service_provider) {
        return std::visit(
                helper::overloaded{
                        [service_provider]([[maybe_unused]] KeyboardControls& keyboard_controls
                        ) mutable -> std::unique_ptr<Input> {
                            auto* const event_dispatcher = &(service_provider->event_dispatcher());
#if defined(__ANDROID__)
                            auto input = std::make_unique<TouchInput>(event_dispatcher);
#elif defined(__SWITCH__)
                            auto input = std::make_unique<JoystickInput>(event_dispatcher);
#else
                            auto input = std::make_unique<KeyboardInput>(keyboard_controls, event_dispatcher);
#endif
                            return input;
                        },
                },
                service_provider->settings().controls
        );
    }

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
                throw std::runtime_error{ "failed in getting display mode: " + std::string{ SDL_GetError() } };
            }
            if (mode.refresh_rate != 0) {
                target_fps = static_cast<u32>(mode.refresh_rate);
            }
        }

        return target_fps;
    }


} // namespace


[[nodiscard]] std::vector<input::AdditionalInfo> input::get_game_parameters_for_replay(
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

        auto input = std::make_unique<ReplayInput>(recording_reader);

        const auto& header = tetrion_headers.at(tetrion_index);

        const auto seed = header.seed;
        const auto starting_level = header.starting_level;

        const tetrion::StartingParameters starting_parameters = { target_fps, seed, starting_level, tetrion_index,
                                                                  helper::nullopt };

        result.emplace_back(std::move(input), starting_parameters);
    }


    return result;
}


[[nodiscard]] input::AdditionalInfo input::get_single_player_game_parameters(
        ServiceProvider* const service_provider,
        recorder::AdditionalInformation&& information,
        const date::ISO8601Date& date
) {

    auto input = create_input(service_provider);

    const auto starting_level = service_provider->command_line_arguments().starting_level;

    const auto seed = Random::generate_seed();

    const auto target_fps = get_target_fps(service_provider);

    const tetrion::StartingParameters starting_parameters = { target_fps, seed, starting_level, 0 };

    AdditionalInfo result{ std::move(input), starting_parameters };


    auto tetrion_header = create_tetrion_headers_for_one(result);
    std::vector<recorder::TetrionHeader> tetrion_headers{ tetrion_header };

    const auto recording_directory_path = utils::get_root_folder() / constants::recordings_directory;

    if (not std::filesystem::exists(recording_directory_path)) {
        std::filesystem::create_directory(recording_directory_path);
    }

    const auto date_time_str = date.to_string();

    if (not date_time_str.has_value()) {
        throw std::runtime_error{ fmt::format("Erro in date to string conversion: {}", date_time_str.error()) };
    }

    const auto filename = fmt::format("{}.rec", date_time_str.value());
    const auto file_path = recording_directory_path / filename;


    auto recording_writer_create_result =
            recorder::RecordingWriter::get_writer(file_path, std::move(tetrion_headers), std::move(information));
    if (not recording_writer_create_result.has_value()) {
        throw std::runtime_error(recording_writer_create_result.error());
    }

    const auto recording_writer =
            std::make_shared<recorder::RecordingWriter>(std::move(recording_writer_create_result.value()));


    std::get<1>(result).recording_writer = recording_writer;


    return result;
}
