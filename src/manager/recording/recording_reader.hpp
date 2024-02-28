#pragma once

#include "recording.hpp"
#include "tetrion_snapshot.hpp"

#include <filesystem>
#include <fstream>
#include <spdlog/spdlog.h>

namespace recorder {

    enum class ReadError : u8 {
        EndOfFile,
        Incomplete,
        InvalidStream,
    };

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


        template<typename Result>
        using ReadResult = helper::expected<Result, ReadError>;

        template<utils::integral Integral>
        [[nodiscard]] static ReadResult<std::remove_cv_t<Integral>> read_integral_from_file(std::ifstream& file) {
            if (not file) {
                spdlog::error("failed to read data from file");
                return helper::unexpected<ReadError>{ ReadError::InvalidStream };
            }

            std::remove_cv_t<Integral> little_endian_data{};
            file.read(
                    reinterpret_cast<char*>(&little_endian_data), // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
                    sizeof(little_endian_data)
            );
            if (not file) {
                return helper::unexpected<ReadError>{ ReadError::Incomplete };
            }
            return utils::from_little_endian(little_endian_data);
        }

        template<typename Type, usize Size>
        [[nodiscard]] static ReadResult<std::array<Type, Size>> read_array_from_file(std::ifstream& file) {
            if (not file) {
                spdlog::error("failed to read data from file");
                return helper::unexpected<ReadError>{ ReadError::InvalidStream };
            }

            std::array<Type, Size> result{};
            for (decltype(Size) i = 0; i < Size; ++i) {
                const auto read_data = read_integral_from_file<Type>(file);
                if (not read_data.has_value()) {
                    return helper::unexpected<ReadError>{ read_data.error() };
                }
                result.at(i) = read_data.value();
            }
            if (not file) {
                return helper::unexpected<ReadError>{ ReadError::Incomplete };
            }
            return result;
        }

        [[nodiscard]] static ReadResult<TetrionHeader> read_tetrion_header_from_file(std::ifstream& file);

        [[nodiscard]] static ReadResult<Record> read_record_from_file(std::ifstream& file);
    };

} // namespace recorder
