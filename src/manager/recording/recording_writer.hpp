#pragma once

#include "helper.hpp"
#include "helper/expected.hpp"
#include "recording.hpp"
#include "tetrion_core_information.hpp"

#include <filesystem>

namespace recorder {

    struct RecordingWriter : public Recording {
    private:
        std::ofstream m_output_file;
        constexpr static u8 version_number = 1;

        explicit RecordingWriter(std::ofstream&& output_file, std::vector<TetrionHeader>&& tetrion_headers);

    public:
        RecordingWriter(RecordingWriter&& old) noexcept;

        static helper::expected<RecordingWriter, std::string>
        get_writer(const std::filesystem::path& path, std::vector<TetrionHeader>&& tetrion_headers);

        helper::expected<bool, std::string> add_event(
                u8 tetrion_index, // NOLINT(bugprone-easily-swappable-parameters)
                u64 simulation_step_index,
                InputEvent event
        );
        helper::expected<bool, std::string>
        add_snapshot(u8 tetrion_index, u64 simulation_step_index, std::unique_ptr<TetrionCoreInformation> information);

    private:
        static helper::expected<bool, std::string>
        write_tetrion_header_to_file(std::ofstream& file, const TetrionHeader& header);

        static helper::expected<bool, std::string>
        write_checksum_to_file(std::ofstream& file, const std::vector<TetrionHeader>& tetrion_headers);

        template<utils::integral Integral>
        helper::expected<bool, std::string> write(Integral data) {
            const auto result = helper::writer::write_integral_to_file(m_output_file, data);
            if (not result.has_value()) {
                return helper::unexpected<std::string>{ fmt::format("error while writing: {}", result.error()) };
            }

            return true;
        }
    };

} // namespace recorder
