#pragma once

#include "./export_symbols.hpp"

#include "./helper.hpp"

#include "./recording.hpp"
#include "./tetrion_snapshot.hpp"

#include <filesystem>

namespace recorder {

    struct RecordingReader : public Recording {
    private:
        using UnderlyingContainer = std::vector<Record>;

        UnderlyingContainer m_records;
        std::vector<TetrionSnapshot> m_snapshots;

        explicit RecordingReader(
                std::vector<TetrionHeader>&& tetrion_headers,
                AdditionalInformation&& information,
                UnderlyingContainer&& records,
                std::vector<TetrionSnapshot>&& snapshots
        );

    public:
        OOPETRIS_RECORDINGS_EXPORTED RecordingReader(RecordingReader&& old) noexcept;

        OOPETRIS_RECORDINGS_EXPORTED static helper::expected<RecordingReader, std::string> from_path(
                const std::filesystem::path& path
        );

        [[nodiscard]] OOPETRIS_RECORDINGS_EXPORTED const Record& at(usize index) const;

        [[nodiscard]] OOPETRIS_RECORDINGS_EXPORTED usize num_records() const;

        [[nodiscard]] OOPETRIS_RECORDINGS_EXPORTED const UnderlyingContainer& records() const;

        [[nodiscard]] OOPETRIS_RECORDINGS_EXPORTED const std::vector<TetrionSnapshot>& snapshots() const;

        [[nodiscard]] OOPETRIS_RECORDINGS_EXPORTED static helper::
                expected<std::pair<recorder::AdditionalInformation, std::vector<recorder::TetrionHeader>>, std::string>
                is_header_valid(const std::filesystem::path& path);

        // iterator trait
        using iterator = UnderlyingContainer::iterator;               //NOLINT(readability-identifier-naming)
        using const_iterator = UnderlyingContainer::const_iterator;   //NOLINT(readability-identifier-naming)
        using difference_type = UnderlyingContainer::difference_type; //NOLINT(readability-identifier-naming)
        using value_type = UnderlyingContainer::value_type;           //NOLINT(readability-identifier-naming)
        using pointer = UnderlyingContainer::pointer;                 //NOLINT(readability-identifier-naming)
        using reference = UnderlyingContainer::reference;             //NOLINT(readability-identifier-naming)
        using iterator_category = std::bidirectional_iterator_tag;    //NOLINT(readability-identifier-naming)

        [[nodiscard]] OOPETRIS_RECORDINGS_EXPORTED iterator begin();

        [[nodiscard]] OOPETRIS_RECORDINGS_EXPORTED const_iterator begin() const;

        [[nodiscard]] OOPETRIS_RECORDINGS_EXPORTED iterator end();

        [[nodiscard]] OOPETRIS_RECORDINGS_EXPORTED const_iterator end() const;

    private:
        [[nodiscard]] static helper::expected<
                std::tuple<std::ifstream, std::vector<TetrionHeader>, recorder::AdditionalInformation>,
                std::string>
        get_header_from_path(const std::filesystem::path& path);


        [[nodiscard]] static helper::reader::ReadResult<TetrionHeader> read_tetrion_header_from_file(std::ifstream& file
        );

        [[nodiscard]] static helper::reader::ReadResult<Record> read_record_from_file(std::ifstream& file);
    };

    STATIC_ASSERT_WITH_MESSAGE(utils::IsIterator<RecordingReader>::value, "RecordingReader has to be an iterator");

} // namespace recorder
