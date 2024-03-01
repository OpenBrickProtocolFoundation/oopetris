
#include "recording_reader.hpp"

#include <fmt/format.h>
#include <fmt/ranges.h>
#include <magic_enum.hpp>

recorder::RecordingReader::RecordingReader(
        std::vector<TetrionHeader>&& tetrion_headers,
        std::vector<Record>&& records,
        std::vector<TetrionSnapshot>&& snapshots
)
    : Recording{ std::move(tetrion_headers) },
      m_records{ std::move(records) },
      m_snapshots{ std::move(snapshots) } { }


recorder::RecordingReader::RecordingReader(RecordingReader&& old)
    : recorder::RecordingReader{ std::move(old.m_tetrion_headers), std::move(old.m_records),
                                 std::move(old.m_snapshots) } { }


helper::expected<recorder::RecordingReader, std::string> recorder::RecordingReader::from_path(
        const std::filesystem::path& path
) {

    std::ifstream file{ path, std::ios::in | std::ios::binary };
    if (not file) {
        return helper::unexpected<std::string>{
            fmt::format("unable to load recording from file \"{}\"", path.string())
        };
    }

    const auto magic_bytes = helper::reader::read_integral_from_file<decltype(Recording::magic_file_byte)>(file);
    if (not magic_bytes.has_value()) {
        return helper::unexpected<std::string>{ "unable to read magic file bytes from recorded game" };
    }
    if (magic_bytes.value() != Recording::magic_file_byte) {
        return helper::unexpected<std::string>{
            "magic file bytes are not correct, this is either an old format or no recording at all"
        };
    }

    const auto version_number = helper::reader::read_integral_from_file<u8>(file);
    if (not version_number.has_value()) {
        return helper::unexpected<std::string>{ "unable to read recording version from recorded game" };
    }
    if (version_number.value() != 1) {
        return helper::unexpected<std::string>{
            fmt::format("only supported version at the moment is {}, but got {}", 1, version_number.value())
        };
    }

    const auto num_tetrions = helper::reader::read_integral_from_file<u8>(file);
    if (not num_tetrions.has_value()) {
        return helper::unexpected<std::string>{ "unable to read number of tetrions from recorded game" };
    }

    std::vector<TetrionHeader> tetrion_headers{};

    tetrion_headers.reserve(num_tetrions.value());
    for (u8 i = 0; i < num_tetrions.value(); ++i) {
        auto header = read_tetrion_header_from_file(file);
        if (not header.has_value()) {
            throw RecordingError{ "failed to read tetrion header from recorded game" };
        }
        tetrion_headers.push_back(header.value());
    }

    const auto calculated_checksum = Recording::get_header_checksum(version_number.value(), tetrion_headers);

    const auto read_checksum =
            helper::reader::read_array_from_file<decltype(calculated_checksum)::value_type, Sha256Stream::ChecksumSize>(
                    file
            );
    if (not read_checksum.has_value()) {
        return helper::unexpected<std::string>{ "unable to read header checksum from recorded game" };
    }
    if (read_checksum.value() != calculated_checksum) {
        return helper::unexpected<std::string>{ fmt::format(
                "header checksum mismatch, the file was altered: expected {:x} but got {:x}",
                fmt::join(calculated_checksum, ""), fmt::join(read_checksum.value(), "")
        ) };
    }


    std::vector<Record> records{};
    std::vector<TetrionSnapshot> snapshots{};


    while (true) {
        const auto magic_byte = helper::reader::read_integral_from_file<std::underlying_type_t<MagicByte>>(file);
        if (not magic_byte.has_value()) {
            if (magic_byte.error().first == helper::reader::ReadErrorType::Incomplete) {
                break;
            }
            return helper::unexpected<std::string>{ "unable to read magic byte" };
        }

        if (magic_byte.value() == utils::to_underlying(MagicByte::Record)) {
            const auto record = read_record_from_file(file);
            if (not record.has_value()) {
                return helper::unexpected<std::string>{ "invalid record while reading recorded game" };
            }
            records.push_back(record.value());
        } else if (magic_byte.value() == utils::to_underlying(MagicByte::Snapshot)) {
            auto snapshot = TetrionSnapshot::from_istream(file);
            if (not snapshot.has_value()) {
                return helper::unexpected<std::string>{ "error while reading TetrionSnapshot" };
            }
            snapshots.push_back(std::move(snapshot.value()));
        } else {
            return helper::unexpected<std::string>{
                fmt::format("invalid magic byte: {}", static_cast<int>(magic_byte.value()))
            };
        }

        if (not file) {
            break;
        }
    }

    return RecordingReader{ std::move(tetrion_headers), std::move(records), std::move(snapshots) };
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

[[nodiscard]] const std::vector<recorder::Record>& recorder::RecordingReader::records() {
    return m_records;
}
[[nodiscard]] const std::vector<TetrionSnapshot>& recorder::RecordingReader::snapshots() {
    return m_snapshots;
}

[[nodiscard]] const std::vector<TetrionSnapshot>& recorder::RecordingReader::snapshots() const {
    return m_snapshots;
}

[[nodiscard]] helper::reader::ReadResult<recorder::TetrionHeader>
recorder::RecordingReader::read_tetrion_header_from_file(std::ifstream& file) {
    if (not file) {
        return helper::unexpected<helper::reader::ReadError>{
            {helper::reader::ReadErrorType::InvalidStream, "failed to read data from file"}
        };
    }

    const auto seed = helper::reader::read_integral_from_file<decltype(TetrionHeader::seed)>(file);

    if (not seed.has_value()) {
        return helper::unexpected<helper::reader::ReadError>{
            {helper::reader::ReadErrorType::Incomplete, "field 'seed' has no value"}
        };
    }

    const auto starting_level = helper::reader::read_integral_from_file<decltype(TetrionHeader::starting_level)>(file);
    if (not starting_level.has_value()) {
        return helper::unexpected<helper::reader::ReadError>{
            {helper::reader::ReadErrorType::Incomplete, "field 'starting_level' has no value"}
        };
    }

    const auto information = AdditionalInformation::from_istream(file);
    if (not information.has_value()) {
        return helper::unexpected<helper::reader::ReadError>{
            {helper::reader::ReadErrorType::Incomplete, "field 'information' has no value"}
        };
    }

    return TetrionHeader{ .seed = seed.value(),
                          .starting_level = starting_level.value(),
                          .information = information.value() };
}

[[nodiscard]] helper::reader::ReadResult<recorder::Record> recorder::RecordingReader::read_record_from_file(
        std::ifstream& file
) {
    if (not file) {
        return helper::unexpected<helper::reader::ReadError>{
            {helper::reader::ReadErrorType::InvalidStream, "invalid input file stream while trying to read record"}
        };
    }

    const auto tetrion_index = helper::reader::read_integral_from_file<decltype(Record::tetrion_index)>(file);
    if (not tetrion_index.has_value()) {
        return helper::unexpected<helper::reader::ReadError>{
            {helper::reader::ReadErrorType::EndOfFile, "the field 'tetrion_index' is missing"}
        };
    }

    const auto simulation_step_index =
            helper::reader::read_integral_from_file<decltype(Record::simulation_step_index)>(file);
    if (not simulation_step_index.has_value()) {
        return helper::unexpected<helper::reader::ReadError>{
            {helper::reader::ReadErrorType::Incomplete, "the field 'simulation_step_index' is missing"}
        };
    }

    const auto event = helper::reader::read_integral_from_file<std::underlying_type_t<InputEvent>>(file);
    if (not event.has_value()) {
        return helper::unexpected<helper::reader::ReadError>{
            {helper::reader::ReadErrorType::Incomplete, "the field 'InputEvent' is missing"}
        };
    }
    if (not file) {
        return helper::unexpected<helper::reader::ReadError>{
            {helper::reader::ReadErrorType::Incomplete, "failed to read data from file"}
        };
    }

    const auto maybe_event = magic_enum::enum_cast<InputEvent>(event.value());
    if (not maybe_event.has_value()) {
        return helper::unexpected<helper::reader::ReadError>{
            {helper::reader::ReadErrorType::Incomplete,
             fmt::format("got invalid enum value for InputEvent: {}", event.value())}
        };
    }

    return Record{
        .tetrion_index = tetrion_index.value(),
        .simulation_step_index = simulation_step_index.value(),
        .event = maybe_event.value(),
    };
}
