

#pragma once


#include "helper/expected.hpp"
#include "helper/types.hpp"
#include "helper/utils.hpp"

#include <filesystem>
#include <fstream>
#include <spdlog/spdlog.h>

namespace helper {

    namespace reader {

        enum class ReadError : u8 {
            EndOfFile,
            Incomplete,
            InvalidStream,
        };


        template<typename Result>
        using ReadResult = helper::expected<Result, ReadError>;

        template<utils::integral Integral>
        [[nodiscard]] ReadResult<std::remove_cv_t<Integral>> read_integral_from_file(std::ifstream& file) {
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
        [[nodiscard]] ReadResult<std::array<Type, Size>> read_array_from_file(std::ifstream& file) {
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

        template<utils::integral Integral>
        [[nodiscard]] helper::optional<Integral> read_from_istream(std::istream& istream) {
            if (not istream) {
                return helper::nullopt;
            }
            auto value = Integral{};
            istream.read(
                    reinterpret_cast<char*>(&value),
                    sizeof(Integral) // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
            );

            if (not istream) {
                return helper::nullopt;
            }

            return utils::from_little_endian(value);
        }

        template<typename Type, usize Size>
        [[nodiscard]] helper::optional<std::array<Type, Size>> read_array_from_istream(std::istream& istream) {
            if (not istream) {
                return helper::nullopt;
            }

            std::array<Type, Size> result{};
            for (decltype(Size) i = 0; i < Size; ++i) {
                const auto read_data = read_from_istream<Type>(istream);
                if (not read_data.has_value()) {
                    return helper::nullopt;
                }
                result.at(i) = read_data.value();
            }

            if (not istream) {
                return helper::nullopt;
            }

            return result;
        }


    } // namespace reader

    namespace writer {

        template<utils::integral Integral>
        void write_integral_to_file(std::ofstream& file, const Integral data) {
            if (not file) {
                spdlog::error("failed to write data \"{}\"", data);
                return;
            }

            const auto little_endian_data = utils::to_little_endian(data);
            file.write(
                    reinterpret_cast<const char*>(
                            &little_endian_data // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
                    ),
                    sizeof(little_endian_data)
            );
        }

        template<typename T>
        void write_vector_to_file(std::ofstream& file, const std::vector<T>& values) {
            for (const auto& value : values) {
                write_integral_to_file<T>(file, value);
            }
        }


        template<utils::integral Integral>
        void append_value(std::vector<char>& vector, const Integral value) {
            const auto little_endian_value = utils::to_little_endian(value);
            const char* const start =
                    reinterpret_cast<const char*>( // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
                            &little_endian_value
                    );
            const char* const end =
                    start + sizeof(little_endian_value); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
            for (const char* pointer = start; pointer < end;
                 ++pointer) { // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                vector.push_back(*pointer);
            }
        }

        template<typename T>
        void append_bytes(std::vector<char>& vector, const std::vector<T>& values) {
            for (const auto& value : values) {
                append_value<T>(vector, value);
            }
        }

    } // namespace writer

} // namespace helper
