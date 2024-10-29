
#pragma once

#if !defined(NDEBUG)
// better json error messages, see https://json.nlohmann.me/api/macros/json_diagnostics/
#define JSON_DIAGNOSTICS 1 // NOLINT(cppcoreguidelines-macro-usage)
#endif

#include <nlohmann/json.hpp>

#include "./expected.hpp"
#include "./windows.hpp"

#include <filesystem>
#include <fmt/format.h>
#include <fstream>
#include <sstream>
#include <string>

// START: general json parser helper

//helper for std::optional json conversion

NLOHMANN_JSON_NAMESPACE_BEGIN
template<typename T>
struct adl_serializer<std::optional<T>> {
    static void to_json(json& obj, const std::optional<T>& opt) {
        if (not opt) {
            obj = nullptr;
        } else {
            obj = *opt; // this will call adl_serializer<T>::to_json which will
                        // find the free function to_json in T's namespace!
        }
    }

    static void from_json(const json& obj, std::optional<T>& opt) {
        if (obj.is_null()) {
            opt = std::nullopt;
        } else {
            opt = obj.template get<T>(); // same as above, but with
                                         // adl_serializer<T>::from_json
        }
    }
};
NLOHMANN_JSON_NAMESPACE_END


// END: general json parser helper

namespace json {

    template<typename T>
    [[nodiscard]] helper::expected<T, std::string> try_parse_json(const std::string& content) noexcept {

        try {
            T result = nlohmann::json::parse(content);
            return result;

        } catch (nlohmann::json::parse_error& parse_error) {
            return helper::unexpected<std::string>{ fmt::format("parse error: {}", parse_error.what()) };
        } catch (nlohmann::json::type_error& type_error) {
            return helper::unexpected<std::string>{ fmt::format("type error: {}", type_error.what()) };
        } catch (nlohmann::json::exception& exception) {
            return helper::unexpected<std::string>{ fmt::format("unknown json exception: {}", exception.what()) };
        } catch (std::exception& exception) {
            return helper::unexpected<std::string>{ fmt::format("unknown exception: {}", exception.what()) };
        }
    }

    template<typename T>
    [[nodiscard]] helper::expected<T, std::string> try_parse_json_file(const std::filesystem::path& file) noexcept {

        if (not std::filesystem::exists(file)) {
            return helper::unexpected<std::string>{ fmt::format("File '{}' doesn't exist", file.string()) };
        }

        std::ifstream file_stream{ file };

        if (not file_stream.is_open()) {
            return helper::unexpected<std::string>{ fmt::format("File '{}' couldn't be opened!", file.string()) };
        }

        std::stringstream result;
        result << file_stream.rdbuf();

        return try_parse_json<T>(result.str());
    }

    template<typename T>
    [[nodiscard]] helper::expected<nlohmann::json, std::string> try_convert_to_json(const T& input) noexcept {

        try {
            nlohmann::json value = input;
            return value;
        } catch (nlohmann::json::type_error& type_error) {
            return helper::unexpected<std::string>{ fmt::format("type error: {}", type_error.what()) };
        } catch (nlohmann::json::exception& exception) {
            return helper::unexpected<std::string>{ fmt::format("unknown json exception: {}", exception.what()) };
        } catch (std::exception& exception) {
            return helper::unexpected<std::string>{ fmt::format("unknown exception: {}", exception.what()) };
        }
    }


    template<typename T>
    [[nodiscard]] helper::expected<std::string, std::string>
    try_json_to_string(const T& type, const bool pretty = false) noexcept {
        try {

            auto value = try_convert_to_json<T>(type);

            if (not value.has_value()) {
                return helper::unexpected<std::string>{ value.error() };
            }

            if (pretty) {
                return value.value().dump(1, '\t');
            }

            return value.value().dump(-1, ' ');

        } catch (nlohmann::json::type_error& type_error) {
            return helper::unexpected<std::string>{ fmt::format("type error: {}", type_error.what()) };
        } catch (nlohmann::json::exception& exception) {
            return helper::unexpected<std::string>{ fmt::format("unknown json exception: {}", exception.what()) };
        } catch (std::exception& exception) {
            return helper::unexpected<std::string>{ fmt::format("unknown exception: {}", exception.what()) };
        }
    }


    template<typename T>
    std::optional<std::string>
    try_write_json_to_file(const std::filesystem::path& file, const T& value, const bool pretty = false) {


        const auto result = json::try_json_to_string<T>(value, pretty);

        if (not result.has_value()) {
            return fmt::format("unable to convert settings to json: {}", result.error());
        }

        std::ofstream file_stream{ file };

        if (not file_stream.is_open()) {
            return fmt::format("File '{}' couldn't be opened!", file.string());
        }

        file_stream << result.value();

        file_stream.close();

        if (file_stream.fail()) {
            return fmt::format("Couldn't write to file '{}' ", file.string());
        }

        return std::nullopt;
    }


    OOPETRIS_CORE_EXPORTED std::string get_json_type(const nlohmann::json::value_t& type);

    OOPETRIS_CORE_EXPORTED [[nodiscard]] bool is_meta_key(const std::string& key);

    OOPETRIS_CORE_EXPORTED void
    check_for_no_additional_keys(const nlohmann::json& obj, const std::vector<std::string>& keys);

} // namespace json
