

#pragma once


#include <core/helper/expected.hpp>
#include <core/helper/types.hpp>
#include <core/helper/utils.hpp>

#include <filesystem>
#include <fmt/format.h>
#include <fstream>
#include <string>
#include <utility>
#include <vector>

namespace helper {

    namespace reader {

        enum class ReadErrorType : u8 {
            EndOfFile,
            Incomplete,
            InvalidStream,
        };


        using ReadError = std::pair<ReadErrorType, std::string>;
        template<typename Result>
        using ReadResult = helper::expected<Result, ReadError>;

        template<utils::integral Integral>
        [[nodiscard]] ReadResult<std::remove_cv_t<Integral>> read_integral_from_file(std::ifstream& file) {
            if (not file) {
                return helper::unexpected<ReadError>{
                    { ReadErrorType::InvalidStream, "failed to read data from file (before reading)" }
                };
            }

            std::remove_cv_t<Integral> little_endian_data{};
            file.read(
                    reinterpret_cast<char*>(&little_endian_data), // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
                    sizeof(little_endian_data)
            );

            if (not file) {
                return helper::unexpected<ReadError>{
                    { ReadErrorType::Incomplete, "failed to read data from file (after reading)" }
                };
            }

            return utils::from_little_endian(little_endian_data);
        }

        template<typename Type, usize Size>
        [[nodiscard]] ReadResult<std::array<Type, Size>> read_array_from_file(std::ifstream& file) {
            if (not file) {
                return helper::unexpected<ReadError>{
                    { ReadErrorType::InvalidStream, "failed to read data from file (before reading)" }
                };
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
                return helper::unexpected<ReadError>{
                    { ReadErrorType::Incomplete, "failed to read data from file (after reading)" }
                };
            }

            return result;
        }

        template<utils::integral Integral>
        [[nodiscard]] std::optional<Integral> read_from_istream(std::istream& istream) {
            if (not istream) {
                return std::nullopt;
            }
            auto value = Integral{};
            istream.read(
                    reinterpret_cast<char*>(&value), // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
                    sizeof(Integral)
            );

            if (not istream) {
                return std::nullopt;
            }

            return utils::from_little_endian(value);
        }

        template<typename Type, usize Size>
        [[nodiscard]] std::optional<std::array<Type, Size>> read_array_from_istream(std::istream& istream) {
            if (not istream) {
                return std::nullopt;
            }

            std::array<Type, Size> result{};
            for (decltype(Size) i = 0; i < Size; ++i) {
                const auto read_data = read_from_istream<Type>(istream);
                if (not read_data.has_value()) {
                    return std::nullopt;
                }
                result.at(i) = read_data.value();
            }

            if (not istream) {
                return std::nullopt;
            }

            return result;
        }


    } // namespace reader

    namespace writer {

        template<utils::integral Integral>
        helper::expected<bool, std::string> write_integral_to_file(std::ofstream& file, const Integral data) {
            if (not file) {
                return helper::unexpected<std::string>{ fmt::format("failed to write data \"{}\"", data) };
            }

            const auto little_endian_data = utils::to_little_endian(data);
            file.write(
                    reinterpret_cast<const char*>( // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
                            &little_endian_data
                    ),
                    sizeof(little_endian_data)
            );

            return true;
        }

        template<typename T>
        helper::expected<bool, std::string> write_vector_to_file(std::ofstream& file, const std::vector<T>& values) {
            helper::expected<bool, std::string> result{ true };
            for (const auto& value : values) {
                result = write_integral_to_file<T>(file, value);
                if (not result.has_value()) {
                    return helper::unexpected<std::string>{ result.error() };
                }
            }

            return result;
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
