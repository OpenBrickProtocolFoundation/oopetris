#include "recording_writer.hpp"

recorder::RecordingWriter::RecordingWriter(
        const std::filesystem::path& path,
        std::vector<TetrionHeader>&& tetrion_headers
)
    : Recording{ std::move(tetrion_headers) },
      m_output_file{ path, std::ios::out | std::ios::binary },
      m_path{ path } { }


helper::expected<bool, std::string> recorder::RecordingWriter::write_tetrion_headers() {
    if (not m_output_file) {
        return helper::unexpected<std::string>{ fmt::format("failed to open output file \"{}\"", m_path.string()) };
    }

    helper::expected<bool, std::string> result{ true };

    static_assert(sizeof(Recording::magic_file_byte) == 4);
    result = helper::writer::write_integral_to_file(m_output_file, Recording::magic_file_byte);
    if (not result.has_value()) {
        return helper::unexpected<std::string>{ fmt::format("error while writing: {}", result.error()) };
    }

    static_assert(sizeof(RecordingWriter::version_number) == 1);
    result = helper::writer::write_integral_to_file(m_output_file, RecordingWriter::version_number);
    if (not result.has_value()) {
        return helper::unexpected<std::string>{ fmt::format("error while writing: {}", result.error()) };
    }

    result = helper::writer::write_integral_to_file<u8>(m_output_file, static_cast<u8>(m_tetrion_headers.size()));
    if (not result.has_value()) {
        return helper::unexpected<std::string>{ fmt::format("error while writing: {}", result.error()) };
    }

    for (const auto& header : m_tetrion_headers) {
        result = write_tetrion_header_to_file(m_output_file, header);
        if (not result.has_value()) {
            return helper::unexpected<std::string>{ result.error() };
        }
    }

    result = write_checksum_to_file(m_output_file);

    return result;
}

helper::expected<bool, std::string> recorder::RecordingWriter::add_event(
        const u8 tetrion_index, // NOLINT(bugprone-easily-swappable-parameters)
        const u64 simulation_step_index,
        const InputEvent event
) {
    assert(tetrion_index < m_tetrion_headers.size());

    helper::expected<bool, std::string> result{ true };

    static_assert(sizeof(std::underlying_type_t<MagicByte>) == 1);
    result = write(utils::to_underlying(MagicByte::Record));
    if (not result.has_value()) {
        return helper::unexpected<std::string>{ result.error() };
    }

    static_assert(sizeof(decltype(tetrion_index)) == 1);
    result = write(tetrion_index);
    if (not result.has_value()) {
        return helper::unexpected<std::string>{ result.error() };
    }

    static_assert(sizeof(decltype(simulation_step_index)) == 8);
    result = write(simulation_step_index);
    if (not result.has_value()) {
        return helper::unexpected<std::string>{ result.error() };
    }

    static_assert(sizeof(std::underlying_type_t<InputEvent>) == 1);
    result = write(utils::to_underlying(event));

    return result;
}

helper::expected<bool, std::string> recorder::RecordingWriter::add_snapshot(
        const u8 tetrion_index,
        const u64 simulation_step_index,
        std::unique_ptr<TetrionCoreInformation> information
) {

    helper::expected<bool, std::string> result{ true };

    static_assert(sizeof(std::underlying_type_t<MagicByte>) == 1);
    result = write(utils::to_underlying(MagicByte::Snapshot));
    if (not result.has_value()) {
        return helper::unexpected<std::string>{ result.error() };
    }

    const auto snapshot = TetrionSnapshot{ tetrion_index,         information->level,
                                           information->score,    information->lines_cleared,
                                           simulation_step_index, information->mino_stack };

    const auto bytes = snapshot.to_bytes();
    result = helper::writer::write_vector_to_file(m_output_file, bytes);

    return result;
}


helper::expected<bool, std::string>
recorder::RecordingWriter::write_tetrion_header_to_file(std::ofstream& file, const TetrionHeader& header) {
    helper::expected<bool, std::string> result{ true };

    static_assert(sizeof(decltype(header.seed)) == 8);
    result = helper::writer::write_integral_to_file(file, header.seed);
    if (not result.has_value()) {
        return helper::unexpected<std::string>{ result.error() };
    }

    static_assert(sizeof(decltype(header.starting_level)) == 4);
    result = helper::writer::write_integral_to_file(file, header.starting_level);
    if (not result.has_value()) {
        return helper::unexpected<std::string>{ result.error() };
    }

    const auto information_bytes = header.information.to_bytes();
    if (not information_bytes.has_value()) {
        return helper::unexpected<std::string>{ information_bytes.error() };
    }

    result = helper::writer::write_vector_to_file(file, information_bytes.value());

    return result;
}

helper::expected<bool, std::string> recorder::RecordingWriter::write_checksum_to_file(std::ofstream& file) {


    const auto checksum = Recording::get_header_checksum(RecordingWriter::version_number, m_tetrion_headers);
    static_assert(sizeof(decltype(checksum)) == 32);

    helper::expected<bool, std::string> result{ true };

    for (const auto& checksum_byte : checksum) {
        result = helper::writer::write_integral_to_file<u8>(file, checksum_byte);
        if (not result.has_value()) {
            return helper::unexpected<std::string>{ result.error() };
        }
    }

    return result;
}
