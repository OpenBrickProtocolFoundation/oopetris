#include "recording_writer.hpp"

recorder::RecordingWriter::RecordingWriter(
        const std::filesystem::path& path,
        std::vector<TetrionHeader> tetrion_headers
)
    : Recording{ std::move(tetrion_headers) },
      m_output_file{ path, std::ios::out | std::ios::binary } {
    if (not m_output_file) {
        spdlog::error("failed to open output file \"{}\"", path.string());
        throw RecordingError{};
    }
    write_integral_to_file(m_output_file, static_cast<u8>(m_tetrion_headers.size()));

    for (const auto& header : m_tetrion_headers) {
        write_tetrion_header_to_file(m_output_file, header);
    }
}

void recorder::RecordingWriter::add_event(
        const u8 tetrion_index, // NOLINT(bugprone-easily-swappable-parameters)
        const u64 simulation_step_index,
        const InputEvent event
) {
    assert(tetrion_index < m_tetrion_headers.size());
    write(utils::to_underlying(MagicByte::Record));
    write(tetrion_index);
    write(simulation_step_index);
    write(static_cast<u8>(event));
}

void recorder::RecordingWriter::add_snapshot(
        const u8 tetrion_index,
        const u64 simulation_step_index,
        const Tetrion& tetrion
) {
    write(utils::to_underlying(MagicByte::Snapshot));
    const auto snapshot = TetrionSnapshot{ tetrion_index,           tetrion.level(),       tetrion.score(),
                                           tetrion.lines_cleared(), simulation_step_index, tetrion.mino_stack() };
    const auto bytes = snapshot.to_bytes();
    m_output_file.write(bytes.data(), static_cast<std::streamsize>(bytes.size()));
}


void recorder::RecordingWriter::write_integral_to_file(std::ofstream& file, const utils::integral auto data) {
    if (not file) {
        spdlog::error("failed to write data \"{}\"", data);
        return;
    }

    const auto little_endian_data = utils::to_little_endian(data);
    file.write(
            reinterpret_cast<const char*>(&little_endian_data // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
            ),
            sizeof(little_endian_data)
    );
}

void recorder::RecordingWriter::write_tetrion_header_to_file(std::ofstream& file, const TetrionHeader& header) {
    static_assert(sizeof(Random::Seed) == 8);
    write_integral_to_file(file, header.seed);
    write_integral_to_file(file, header.starting_level);
}

void recorder::RecordingWriter::write(const utils::integral auto data) {
    write_integral_to_file(m_output_file, data);
};
