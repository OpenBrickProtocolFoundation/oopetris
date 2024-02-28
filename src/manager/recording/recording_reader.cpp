
#include "recording_reader.hpp"


recorder::RecordingReader::RecordingReader(const std::filesystem::path& path) {
    std::ifstream file{ path, std::ios::in | std::ios::binary };
    if (not file) {
        spdlog::error("unable to load recording from file \"{}\"", path.string());
        throw RecordingError{};
    }
    const auto num_tetrions = read_integral_from_file<u8>(file);
    if (not num_tetrions.has_value()) {
        spdlog::error("unable to read number of tetrions from recorded game");
        throw RecordingError{};
    }

    m_tetrion_headers.reserve(*num_tetrions);
    for (u8 i = 0; i < *num_tetrions; ++i) {
        auto header = read_tetrion_header_from_file(file);
        if (not header) {
            spdlog::error("failed to read tetrion header from recorded game");
            throw RecordingError{};
        }
        m_tetrion_headers.push_back(*header);
    }

    while (true) {
        const auto magic_byte = read_integral_from_file<std::underlying_type_t<MagicByte>>(file);
        if (not magic_byte.has_value()) {
            if (magic_byte.error() == ReadError::InvalidStream) {
                spdlog::error("unable to read magic byte");
                throw RecordingError{};
            }
            break;
        }
        if (*magic_byte == utils::to_underlying(MagicByte::Record)) {
            const auto record = read_record_from_file(file);
            if (not record.has_value()) {
                if (record.error() == ReadError::EndOfFile) {
                    // finished reading
                    break;
                }
                spdlog::error("invalid record while reading recorded game");
                throw RecordingError{};
            }
            m_records.push_back(*record);
        } else if (*magic_byte == utils::to_underlying(MagicByte::Snapshot)) {
            auto snapshot = TetrionSnapshot{ file }; // TODO: handle exception
            m_snapshots.push_back(std::move(snapshot));
        } else {
            spdlog::error("invalid magic byte: {}", static_cast<int>(*magic_byte));
            throw RecordingError{};
        }
    }
}

[[nodiscard]] const recorder::Record& recorder::RecordingReader::at(const usize index) const {
    return m_records.at(index);
}

[[nodiscard]] usize recorder::RecordingReader::num_records() const {
    return m_records.size();
}

[[nodiscard]] auto recorder::RecordingReader::begin() const {
    return m_records.cbegin();
}

[[nodiscard]] auto recorder::RecordingReader::end() const {
    return m_records.cend();
}

[[nodiscard]] const std::vector<TetrionSnapshot>& recorder::RecordingReader::snapshots() const {
    return m_snapshots;
}

[[nodiscard]] recorder::RecordingReader::ReadResult<recorder::TetrionHeader>
recorder::RecordingReader::read_tetrion_header_from_file(std::ifstream& file) {
    if (not file) {
        spdlog::error("failed to read data from file");
        return helper::unexpected<ReadError>{ ReadError::InvalidStream };
    }

    const auto seed = read_integral_from_file<decltype(TetrionHeader::seed)>(file);

    if (not seed.has_value()) {
        return helper::unexpected<ReadError>{ ReadError::Incomplete };
    }

    const auto starting_level = read_integral_from_file<decltype(TetrionHeader::starting_level)>(file);
    if (not starting_level.has_value()) {
        return helper::unexpected<ReadError>{ ReadError::Incomplete };
    }

    return TetrionHeader{ .seed = *seed, .starting_level = *starting_level };
}

[[nodiscard]] recorder::RecordingReader::ReadResult<recorder::Record> recorder::RecordingReader::read_record_from_file(
        std::ifstream& file
) {
    if (not file) {
        spdlog::error("invalid input file stream while trying to read record");
        return helper::unexpected<ReadError>{ ReadError::InvalidStream };
    }

    const auto tetrion_index = read_integral_from_file<decltype(Record::tetrion_index)>(file);
    if (not tetrion_index.has_value()) {
        return helper::unexpected<ReadError>{ ReadError::EndOfFile };
    }

    const auto simulation_step_index = read_integral_from_file<decltype(Record::simulation_step_index)>(file);
    if (not simulation_step_index.has_value()) {
        return helper::unexpected<ReadError>{ ReadError::Incomplete };
    }

    const auto event = read_integral_from_file<u8>(file);
    if (not file) {
        return helper::unexpected<ReadError>{ ReadError::Incomplete };
    }

    return Record{
        .tetrion_index = *tetrion_index,
        .simulation_step_index = *simulation_step_index,
        .event = static_cast<InputEvent>(*event), // TODO: validation
    };
}
