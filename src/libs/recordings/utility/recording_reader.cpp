
#include <core/helper/magic_enum_wrapper.hpp>

#include "./additional_information.hpp"
#include "./recording_reader.hpp"

#include <fmt/format.h>
#include <fmt/ranges.h>
#include <tuple>

recorder::RecordingReader::RecordingReader(
        std::vector<TetrionHeader>&& tetrion_headers,
        AdditionalInformation&& information,
        UnderlyingContainer&& records,
        std::vector<TetrionSnapshot>&& snapshots
)
    : Recording{ std::move(tetrion_headers), std::move(information) },
      m_records{ std::move(records) },
      m_snapshots{ std::move(snapshots) } { }


recorder::RecordingReader::RecordingReader(RecordingReader&& old) noexcept
    : recorder::RecordingReader{ std::move(old.m_tetrion_headers), std::move(old.m_information),
                                 std::move(old.m_records), std::move(old.m_snapshots) } { }


helper::expected<
        std::tuple<std::ifstream, std::vector<recorder::TetrionHeader>, recorder::AdditionalInformation>,
        std::string>
recorder::RecordingReader::get_header_from_path(const std::filesystem::path& path) {

    std::ifstream file{ path, std::ios::in | std::ios::binary };
    if (not file) {
        return helper::unexpected<std::string>{
            fmt::format("unable to load recording from file \"{}\"", path.string())
        };
    }

    const auto magic_bytes =
            helper::reader::read_integral_from_file<decltype(constants::recording::magic_file_byte)>(file);
    if (not magic_bytes.has_value()) {
        return helper::unexpected<std::string>{ "unable to read magic file bytes from recorded game" };
    }
    if (magic_bytes.value() != constants::recording::magic_file_byte) {
        return helper::unexpected<std::string>{
            "magic file bytes are not correct, this is either an old format or no recording at all"
        };
    }

    const auto version_number = helper::reader::read_integral_from_file<u8>(file);
    if (not version_number.has_value()) {
        return helper::unexpected<std::string>{ "unable to read recording version from recorded game" };
    }
    if (version_number.value() != Recording::current_supported_version_number) {
        return helper::unexpected<std::string>{ fmt::format(
                "only supported version at the moment is {}, but got {}", Recording::current_supported_version_number,
                version_number.value()
        ) };
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
            return helper::unexpected<std::string>{ "failed to read tetrion header from recorded game" };
        }
        tetrion_headers.push_back(header.value());
    }


    auto information = AdditionalInformation::from_istream(file);
    if (not information.has_value()) {
        return helper::unexpected<std::string>{ { "failed to read AdditionalInformation from recorded game" } };
    }


    const auto calculated_checksum =
            Recording::get_header_checksum(version_number.value(), tetrion_headers, information.value());

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

    return std::make_tuple<std::ifstream, std::vector<TetrionHeader>, AdditionalInformation>(
            std::move(file), std::move(tetrion_headers), std::move(information.value())
    );
}

helper::expected<recorder::RecordingReader, std::string> recorder::RecordingReader::from_path(
        const std::filesystem::path& path
) {

    auto header = get_header_from_path(path);
    if (not header.has_value()) {
        return helper::unexpected<std::string>{ header.error() };
    }


    auto [file, tetrion_headers, information] = std::move(header.value());


    std::vector<Record> records{};
    std::vector<TetrionSnapshot> snapshots{};
    //TODO(Totto): when using larger files and recordings, we should stream the data and discard used, to far away data, to not load everything into memory at once

    while (true) {

        const auto magic_byte = helper::reader::read_integral_from_file<std::underlying_type_t<MagicByte>>(file);
        if (not magic_byte.has_value()) {
            // we have finished and it's not an error, if we detect an end here (due to the lazy nature of filestreams, the end is only detected after trying to read, and not earlier  after the whole record is read)
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

    return RecordingReader{ std::move(tetrion_headers), std::move(information), std::move(records),
                            std::move(snapshots) };
}

[[nodiscard]] const recorder::Record& recorder::RecordingReader::at(const usize index) const {
    return m_records.at(index);
}

[[nodiscard]] usize recorder::RecordingReader::num_records() const {
    return m_records.size();
}

[[nodiscard]] recorder::RecordingReader::iterator recorder::RecordingReader::begin() {
    return m_records.begin();
}

[[nodiscard]] recorder::RecordingReader::const_iterator recorder::RecordingReader::begin() const {
    return m_records.cbegin();
}

[[nodiscard]] recorder::RecordingReader::iterator recorder::RecordingReader::end() {
    return m_records.end();
}

[[nodiscard]] recorder::RecordingReader::const_iterator recorder::RecordingReader::end() const {
    return m_records.cend();
}

[[nodiscard]] const std::vector<recorder::Record>& recorder::RecordingReader::records() const {
    return m_records;
}

[[nodiscard]] const std::vector<TetrionSnapshot>& recorder::RecordingReader::snapshots() const {
    return m_snapshots;
}


[[nodiscard]] helper::
        expected<std::pair<recorder::AdditionalInformation, std::vector<recorder::TetrionHeader>>, std::string>
        recorder::RecordingReader::is_header_valid(const std::filesystem::path& path) {

    auto header = get_header_from_path(path);

    if (header.has_value()) {
        auto [_, headers, information] = std::move(header.value());
        return std::make_pair<recorder::AdditionalInformation, std::vector<recorder::TetrionHeader>>(
                std::move(information), std::move(headers)
        );
    }

    return helper::unexpected<std::string>{ header.error() };
}


[[nodiscard]] helper::reader::ReadResult<recorder::TetrionHeader>
recorder::RecordingReader::read_tetrion_header_from_file(std::ifstream& file) {
    if (not file) {
        return helper::unexpected<helper::reader::ReadError>{
            { helper::reader::ReadErrorType::InvalidStream, "failed to read data from file" }
        };
    }

    const auto seed = helper::reader::read_integral_from_file<decltype(TetrionHeader::seed)>(file);

    if (not seed.has_value()) {
        return helper::unexpected<helper::reader::ReadError>{
            { helper::reader::ReadErrorType::Incomplete, "field 'seed' has no value" }
        };
    }

    const auto starting_level = helper::reader::read_integral_from_file<decltype(TetrionHeader::starting_level)>(file);
    if (not starting_level.has_value()) {
        return helper::unexpected<helper::reader::ReadError>{
            { helper::reader::ReadErrorType::Incomplete, "field 'starting_level' has no value" }
        };
    }

    return TetrionHeader{ seed.value(), starting_level.value() };
}

[[nodiscard]] helper::reader::ReadResult<recorder::Record> recorder::RecordingReader::read_record_from_file(
        std::ifstream& file
) {
    if (not file) {
        return helper::unexpected<helper::reader::ReadError>{
            { helper::reader::ReadErrorType::InvalidStream, "invalid input file stream while trying to read record" }
        };
    }

    const auto tetrion_index = helper::reader::read_integral_from_file<decltype(Record::tetrion_index)>(file);
    if (not tetrion_index.has_value()) {
        return helper::unexpected<helper::reader::ReadError>{
            { helper::reader::ReadErrorType::EndOfFile, "the field 'tetrion_index' is missing" }
        };
    }

    const auto simulation_step_index =
            helper::reader::read_integral_from_file<decltype(Record::simulation_step_index)>(file);
    if (not simulation_step_index.has_value()) {
        return helper::unexpected<helper::reader::ReadError>{
            { helper::reader::ReadErrorType::Incomplete, "the field 'simulation_step_index' is missing" }
        };
    }

    const auto event = helper::reader::read_integral_from_file<std::underlying_type_t<InputEvent>>(file);
    if (not event.has_value()) {
        return helper::unexpected<helper::reader::ReadError>{
            { helper::reader::ReadErrorType::Incomplete, "the field 'InputEvent' is missing" }
        };
    }
    if (not file) {
        return helper::unexpected<helper::reader::ReadError>{
            { helper::reader::ReadErrorType::Incomplete, "failed to read data from file" }
        };
    }

    const auto maybe_event = magic_enum::enum_cast<InputEvent>(event.value());
    if (not maybe_event.has_value()) {
        return helper::unexpected<helper::reader::ReadError>{
            { helper::reader::ReadErrorType::Incomplete,
             fmt::format("got invalid enum value for InputEvent: {}", event.value()) }
        };
    }

    return Record{
        .tetrion_index = tetrion_index.value(),
        .simulation_step_index = simulation_step_index.value(),
        .event = maybe_event.value(),
    };
}
