
#pragma once

#ifdef DEBUG_BUILD
// better json error messages, see https://json.nlohmann.me/api/macros/json_diagnostics/
#define JSON_DIAGNOSTICS 1 // NOLINT(cppcoreguidelines-macro-usage)
#endif
#include <nlohmann/json.hpp>

#include <filesystem>
#include <fmt/format.h>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tl/expected.hpp>

namespace json {


    template<typename T>
    [[nodiscard]] tl::expected<T, std::string> try_parse_json(const std::string& content) {

        try {
            T result = nlohmann::json::parse(content);
            return result;

        } catch (nlohmann::json::parse_error& parse_error) {
            return tl::make_unexpected(fmt::format("parse error: {}", parse_error.what()));
        } catch (nlohmann::json::type_error& type_error) {
            return tl::make_unexpected(fmt::format("type error: {}", type_error.what()));
        } catch (nlohmann::json::exception& exception) {
            return tl::make_unexpected(fmt::format("unknown json exception: {}", exception.what()));
        } catch (std::exception& exception) {
            return tl::make_unexpected(fmt::format("unknown exception: {}", exception.what()));
        }
    }


    template<typename T>
    [[nodiscard]] tl::expected<T, std::string> try_parse_json_file(const std::filesystem::path& file) {

        if (not std::filesystem::exists(file)) {
            return tl::make_unexpected(fmt::format("File '{}' doesn't exist", file.string()));
        }

        std::ifstream file_stream{ file };

        if (!file_stream.is_open()) {
            return tl::make_unexpected(fmt::format("File '{}' couldn't be opened!", file.string()));
        }

        std::stringstream result;
        result << file_stream.rdbuf();

        return try_parse_json<T>(result.str());
    }


    std::string get_json_type(const nlohmann::json::value_t& type);

    void check_for_no_additional_keys(const nlohmann::json& j, const std::vector<std::string>& keys);


} // namespace json

namespace helpers {

    template<class... Ts>
    struct overloaded : Ts... {
        using Ts::operator()...;
    };
    template<class... Ts>
    overloaded(Ts...) -> overloaded<Ts...>;
} // namespace helpers
