#include "./recording_writer.hpp"
#include "./recording.hpp"
#include "./tetrion_snapshot.hpp"

recorder::RecordingWriter::RecordingWriter(
        std::ofstream&& output_file,
        std::vector<TetrionHeader>&& tetrion_headers,
        AdditionalInformation&& information
)
    : Recording{ std::move(tetrion_headers), std::move(information) },
      m_output_file{ std::move(output_file) } { }


recorder::RecordingWriter::RecordingWriter(RecordingWriter&& old) noexcept
    : recorder::RecordingWriter{ std::move(old.m_output_file), std::move(old.m_tetrion_headers),
                                 std::move(old.m_information) } { }


helper::expected<recorder::RecordingWriter, std::string> recorder::RecordingWriter::get_writer(
        const std::filesystem::path& path,
        std::vector<TetrionHeader>&& tetrion_headers,
        AdditionalInformation&& information,
        bool overwrite
) {
    auto mode = std::ios::out | std::ios::binary;
    if (overwrite) {
        if (std::filesystem::exists(path)) {
            return helper::unexpected<std::string>{
                fmt::format("file already exists, not overwriting it: \"{}\"", path.string())
            };
        }
    } else {
        mode |= std::ios::trunc;
    }


    auto output_file = std::ofstream{ path, mode };

    if (not output_file) {
        return helper::unexpected<std::string>{ fmt::format("failed to open output file \"{}\"", path.string()) };
    }

    helper::expected<void, std::string> result{};

    static_assert(sizeof(constants::recording::magic_file_byte) == 4);
    result = helper::writer::write_integral_to_file(output_file, constants::recording::magic_file_byte);
    if (not result.has_value()) {
        return helper::unexpected<std::string>{ fmt::format("error while writing: {}", result.error()) };
    }

    static_assert(sizeof(Recording::current_supported_version_number) == 1);
    result = helper::writer::write_integral_to_file(output_file, Recording::current_supported_version_number);
    if (not result.has_value()) {
        return helper::unexpected<std::string>{ fmt::format("error while writing: {}", result.error()) };
    }

    result = helper::writer::write_integral_to_file<u8>(output_file, static_cast<u8>(tetrion_headers.size()));
    if (not result.has_value()) {
        return helper::unexpected<std::string>{ fmt::format("error while writing: {}", result.error()) };
    }

    for (const auto& header : tetrion_headers) {
        result = write_tetrion_header_to_file(output_file, header);
        if (not result.has_value()) {
            return helper::unexpected<std::string>{ result.error() };
        }
    }


    const auto information_bytes = information.to_bytes();
    if (not information_bytes.has_value()) {
        return helper::unexpected<std::string>{ information_bytes.error() };
    }

    result = helper::writer::write_vector_to_file(output_file, information_bytes.value());

    if (not result.has_value()) {
        return helper::unexpected<std::string>{ fmt::format("error while writing: {}", result.error()) };
    }

    result = write_checksum_to_file(output_file, tetrion_headers, information);
    if (not result.has_value()) {
        return helper::unexpected<std::string>{ fmt::format("error while writing: {}", result.error()) };
    }

    return RecordingWriter{ std::move(output_file), std::move(tetrion_headers), std::move(information) };
}

helper::expected<void, std::string> recorder::RecordingWriter::add_record(
        const u8 tetrion_index, // NOLINT(bugprone-easily-swappable-parameters)
        const u64 simulation_step_index,
        const InputEvent event
) {
    assert(tetrion_index < m_tetrion_headers.size());

    helper::expected<void, std::string> result{};

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

helper::expected<void, std::string> recorder::RecordingWriter::add_snapshot(
        const u64 simulation_step_index,
        std::unique_ptr<TetrionCoreInformation> information
) {

    helper::expected<void, std::string> result{};

    static_assert(sizeof(std::underlying_type_t<MagicByte>) == 1);
    result = write(utils::to_underlying(MagicByte::Snapshot));
    if (not result.has_value()) {
        return helper::unexpected<std::string>{ result.error() };
    }

    const auto snapshot = TetrionSnapshot{ information->tetrion_index, information->level,    information->score,
                                           information->lines_cleared, simulation_step_index, information->mino_stack };

    const auto bytes = snapshot.to_bytes();
    result = helper::writer::write_vector_to_file(m_output_file, bytes);

    return result;
}


helper::expected<void, std::string>
recorder::RecordingWriter::write_tetrion_header_to_file(std::ofstream& file, const TetrionHeader& header) {
    helper::expected<void, std::string> result{};

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

    return result;
}

helper::expected<void, std::string> recorder::RecordingWriter::write_checksum_to_file(
        std::ofstream& file,
        const std::vector<TetrionHeader>& tetrion_headers,
        const AdditionalInformation& information
) {

    const auto checksum =
            Recording::get_header_checksum(Recording::current_supported_version_number, tetrion_headers, information);
    static_assert(sizeof(decltype(checksum)) == 32);

    helper::expected<void, std::string> result{};

    for (const auto& checksum_byte : checksum) {
        result = helper::writer::write_integral_to_file<u8>(file, checksum_byte);
        if (not result.has_value()) {
            return helper::unexpected<std::string>{ result.error() };
        }
    }

    return result;
}
