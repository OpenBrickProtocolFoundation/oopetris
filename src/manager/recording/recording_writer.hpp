#pragma once

#include "helper.hpp"
#include "recording.hpp"
#include "tetrion_snapshot.hpp"

#include <filesystem>
#include <spdlog/spdlog.h>


namespace recorder {

    struct RecordingWriter : public Recording {
    private:
        std::ofstream m_output_file;
        constexpr static u8 version_number = 1;

    public:
        explicit RecordingWriter(const std::filesystem::path& path, std::vector<TetrionHeader> tetrion_headers);

        void add_event(
                u8 tetrion_index, // NOLINT(bugprone-easily-swappable-parameters)
                u64 simulation_step_index,
                InputEvent event
        );

        void add_snapshot(u8 tetrion_index, u64 simulation_step_index, const Tetrion& tetrion);

    private:
        static void write_tetrion_header_to_file(std::ofstream& file, const TetrionHeader& header);

        void write_checksum_to_file(std::ofstream& file);

        template<utils::integral Integral>
        void write(Integral data) {
            helper::writer::write_integral_to_file(m_output_file, data);
        }
    };

} // namespace recorder
