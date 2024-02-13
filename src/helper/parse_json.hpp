
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
#include <tl/optional.hpp>

// START: general json parser helper

//helper for tl::optional json conversion

NLOHMANN_JSON_NAMESPACE_BEGIN
template<typename T>
struct adl_serializer<tl::optional<T>> {
    static void to_json(json& j, const tl::optional<T>& opt) {
        if (not opt) {
            j = nullptr;
        } else {
            j = *opt; // this will call adl_serializer<T>::to_json which will
                      // find the free function to_json in T's namespace!
        }
    }

    static void from_json(const json& j, tl::optional<T>& opt) {
        if (j.is_null()) {
            opt = tl::nullopt;
        } else {
            opt = j.template get<T>(); // same as above, but with
                                       // adl_serializer<T>::from_json
        }
    }
};
NLOHMANN_JSON_NAMESPACE_END


// END: general json parser helper

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


    template<typename T>
    [[nodiscard]] tl::expected<std::string, std::string> try_json_to_string(const T& type) {
        try {

            const nlohmann::json value = type;
            return value.dump(-1, ' ', false);

        } catch (nlohmann::json::type_error& type_error) {
            return tl::make_unexpected(fmt::format("type error: {}", type_error.what()));
        } catch (nlohmann::json::exception& exception) {
            return tl::make_unexpected(fmt::format("unknown json exception: {}", exception.what()));
        } catch (std::exception& exception) {
            return tl::make_unexpected(fmt::format("unknown exception: {}", exception.what()));
        }
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
