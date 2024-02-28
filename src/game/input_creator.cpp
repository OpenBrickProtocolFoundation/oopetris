

#include "input_creator.hpp"
#include "helper/command_line_arguments.hpp"
#include "platform/replay_input.hpp"

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
                            const auto event_dispatcher = &(service_provider->event_dispatcher());
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


    [[nodiscard]] std::vector<Recording::TetrionHeader> create_tetrion_headers(
            const std::vector<input::AdditionalInfo>& infos
    ) {
        const auto num_tetrions = infos.size();
        std::vector<Recording::TetrionHeader> headers{};
        headers.reserve(num_tetrions);
        for (u8 tetrion_index = 0; tetrion_index < num_tetrions; ++tetrion_index) {
            const auto& info = std::get<1>(infos.at(tetrion_index));
            headers.push_back(Recording::TetrionHeader{ .seed = info.seed, .starting_level = info.starting_level });
        }
        return headers;
    }

} // namespace


namespace input {
    [[nodiscard]] std::vector<AdditionalInfo> get_game_parameters(ServiceProvider* service_provider, u32 amount) {

        std::vector<AdditionalInfo> result{};

        const auto target_fps = service_provider->command_line_arguments().target_fps;

        if (const auto recording_path = service_provider->command_line_arguments().recording_path;
            recording_path.has_value()) {

            const auto recording_reader = std::make_shared<RecordingReader>(recording_path.value());
            const auto tetrion_headers = recording_reader->tetrion_headers();

            if (tetrion_headers.size() != amount) {
                throw std::runtime_error(fmt::format(
                        "Can't load recording with different player amount than expected: the recording has {} but "
                        "expected {}",
                        tetrion_headers.size(), amount
                ));
            }

            for (u32 i = 0; i < amount; ++i) {

                auto input = std::make_unique<ReplayInput>(recording_reader);


                const auto seed = tetrion_headers.at(i).seed;
                const auto starting_level = tetrion_headers.at(i).starting_level;

                const tetrion::StartingParameters starting_parameters = { target_fps, seed, starting_level, i,
                                                                          helper::nullopt };

                result.emplace_back(std::move(input), starting_parameters);
            }


        } else {

            for (u32 i = 0; i < amount; ++i) {

                //TODO: here we have to create different inputs, since the same won't work e.g. in local multiplayer etc.
                auto input = create_input(service_provider);

                const auto starting_level = service_provider->command_line_arguments().starting_level;

                const auto seed = Random::generate_seed();

                const tetrion::StartingParameters starting_parameters = { target_fps, seed, starting_level, i,
                                                                          helper::nullopt };

                result.emplace_back(std::move(input), starting_parameters);
            }


            const auto tetrion_headers = create_tetrion_headers(result);

            static constexpr auto recordings_directory = "recordings";
            const auto recording_directory_path = utils::get_root_folder() / recordings_directory;

            if (not std::filesystem::exists(recording_directory_path)) {
                std::filesystem::create_directory(recording_directory_path);
            }

            const auto filename = fmt::format("{}.rec", utils::current_date_time_iso8601());
            const auto file_path = recording_directory_path / filename;

            const auto recording_writer = std::make_shared<RecordingWriter>(file_path, std::move(tetrion_headers));

            for (auto& res : result) {
                std::get<1>(res).recording_writer = recording_writer;
            }
        }

        return result;
    }

} // namespace input
