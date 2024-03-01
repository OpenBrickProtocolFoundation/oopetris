#include "recording_writer.hpp"

recorder::RecordingWriter::RecordingWriter(
        const std::filesystem::path& path,
        std::vector<TetrionHeader> tetrion_headers
)
    : Recording{ std::move(tetrion_headers) },
      m_output_file{ path, std::ios::out | std::ios::binary } {
    if (not m_output_file) {
        throw RecordingError{ fmt::format("failed to open output file \"{}\"", path.string()) };
    }

    static_assert(sizeof(Recording::magic_file_byte) == 4);
    helper::writer::write_integral_to_file(m_output_file, Recording::magic_file_byte);

    static_assert(sizeof(RecordingWriter::version_number) == 1);
    helper::writer::write_integral_to_file(m_output_file, RecordingWriter::version_number);

    helper::writer::write_integral_to_file<u8>(m_output_file, static_cast<u8>(m_tetrion_headers.size()));

    for (const auto& header : m_tetrion_headers) {
        write_tetrion_header_to_file(m_output_file, header);
    }

    write_checksum_to_file(m_output_file);
}

void recorder::RecordingWriter::add_event(
        const u8 tetrion_index, // NOLINT(bugprone-easily-swappable-parameters)
        const u64 simulation_step_index,
        const InputEvent event
) {
    assert(tetrion_index < m_tetrion_headers.size());

    static_assert(sizeof(std::underlying_type_t<MagicByte>) == 1);
    write(utils::to_underlying(MagicByte::Record));

    static_assert(sizeof(decltype(tetrion_index)) == 1);
    write(tetrion_index);

    static_assert(sizeof(decltype(simulation_step_index)) == 8);
    write(simulation_step_index);

    static_assert(sizeof(std::underlying_type_t<InputEvent>) == 1);
    write(utils::to_underlying(event));
}

void recorder::RecordingWriter::add_snapshot(
        const u8 tetrion_index,
        const u64 simulation_step_index,
        std::unique_ptr<TetrionCoreInformation> information
) {

    static_assert(sizeof(std::underlying_type_t<MagicByte>) == 1);
    write(utils::to_underlying(MagicByte::Snapshot));
    const auto snapshot = TetrionSnapshot{ tetrion_index,         information->level,
                                           information->score,    information->lines_cleared,
                                           simulation_step_index, information->mino_stack };

    const auto bytes = snapshot.to_bytes();
    helper::writer::write_vector_to_file(m_output_file, bytes);
}


void recorder::RecordingWriter::write_tetrion_header_to_file(std::ofstream& file, const TetrionHeader& header) {
    static_assert(sizeof(decltype(header.seed)) == 8);
    helper::writer::write_integral_to_file(file, header.seed);
    static_assert(sizeof(decltype(header.starting_level)) == 4);
    helper::writer::write_integral_to_file(file, header.starting_level);
    const auto information_bytes = header.information.to_bytes();
    helper::writer::write_vector_to_file(file, information_bytes);
}

void recorder::RecordingWriter::write_checksum_to_file(std::ofstream& file) {


    const auto checksum = Recording::get_header_checksum(RecordingWriter::version_number, m_tetrion_headers);
    static_assert(sizeof(decltype(checksum)) == 32);

    for (const auto& checksum_byte : checksum) {
        helper::writer::write_integral_to_file<u8>(file, checksum_byte);
    }
}
