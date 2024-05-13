

#include "parse_json.hpp"


std::string json::get_json_type(const nlohmann::json::value_t& type) {
    switch (type) {

        case nlohmann::json::value_t::null:
            return "null";

        case nlohmann::json::value_t::object:
            return "object";

        case nlohmann::json::value_t::array:
            return "array";

        case nlohmann::json::value_t::string:
            return "string";

        case nlohmann::json::value_t::boolean:
            return "boolean";

        case nlohmann::json::value_t::number_integer:
            return "signed integer";

        case nlohmann::json::value_t::number_unsigned:
            return "unsigned integer";

        case nlohmann::json::value_t::number_float:
            return "float";
        default:
            return "unknown";
    }
}

void json::check_for_no_additional_keys(const nlohmann::json& j, const std::vector<std::string>& keys) {

    if (not j.is_object()) {
        throw nlohmann::json::type_error::create(
                302, fmt::format("expected an object, but got type '{}'", get_json_type(j.type())), &j
        );
    }

    const auto& object = j.get<nlohmann::json::object_t>();


    for (const auto& [key, _] : object) {
        if (std::ranges::find(keys, key) == keys.cend()) {
            throw nlohmann::json::type_error::create(
                    302, fmt::format("object may only contain expected keys, but contained '{}'", key), &j
            );
        }
    }
}
