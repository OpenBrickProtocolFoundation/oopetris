#pragma once

#include "game/tetrion_snapshot.hpp"
#include "recording.hpp"

#include <filesystem>
#include <fstream>
#include <spdlog/spdlog.h>


namespace recorder {

    struct RecordingWriter : public Recording {
    private:
        std::ofstream m_output_file;
        constexpr static u8 version_number = 1;

    public:
        explicit RecordingWriter(const std::filesystem::path& path, std::vector<TetrionHeader> tetrion_headers);

        void add_event(
                const u8 tetrion_index, // NOLINT(bugprone-easily-swappable-parameters)
                const u64 simulation_step_index,
                const InputEvent event
        );

        void add_snapshot(const u8 tetrion_index, const u64 simulation_step_index, const Tetrion& tetrion);

    private:
        static void write_integral_to_file(std::ofstream& file, const utils::integral auto data);

        static void write_tetrion_header_to_file(std::ofstream& file, const TetrionHeader& header);

        void write_checksum_to_file(std::ofstream& file);

        void write(const utils::integral auto data);
    };

} // namespace recorder
