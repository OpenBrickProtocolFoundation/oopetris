#pragma once

#include "helper.hpp"
#include "recording.hpp"
#include "tetrion_snapshot.hpp"

#include <filesystem>
#include <spdlog/spdlog.h>

namespace recorder {

    struct RecordingReader : public Recording {
        std::vector<Record> m_records;
        std::vector<TetrionSnapshot> m_snapshots;

    public:
        explicit RecordingReader(const std::filesystem::path& path);

        [[nodiscard]] const Record& at(const usize index) const;
        [[nodiscard]] usize num_records() const;
        [[nodiscard]] auto begin() const;
        [[nodiscard]] auto end() const;
        [[nodiscard]] const std::vector<TetrionSnapshot>& snapshots() const;

        [[nodiscard]] static helper::reader::ReadResult<TetrionHeader> read_tetrion_header_from_file(std::ifstream& file
        );

        [[nodiscard]] static helper::reader::ReadResult<Record> read_record_from_file(std::ifstream& file);
    };

} // namespace recorder
