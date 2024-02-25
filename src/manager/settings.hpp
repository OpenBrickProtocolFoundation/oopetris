#pragma once

#include "helper/magic_enum_wrapper.hpp"
#include "helper/parse_json.hpp"
#include "input/controls.hpp"
#include <array>

#include <fmt/format.h>
#include <string>
#include <variant>

inline constexpr auto max_num_players = 4;

inline void to_json(nlohmann::json& j, const KeyboardControls& controls) {
    j = nlohmann::json{
        { "rotate_left",  magic_enum::enum_name(controls.rotate_left)},
        {"rotate_right", magic_enum::enum_name(controls.rotate_right)},
        {   "move_left",    magic_enum::enum_name(controls.move_left)},
        {  "move_right",   magic_enum::enum_name(controls.move_right)},
        {   "move_down",    magic_enum::enum_name(controls.move_down)},
        {        "drop",         magic_enum::enum_name(controls.drop)},
        {        "hold",         magic_enum::enum_name(controls.hold)},
    };
}


inline KeyCode get_key_code_safe(const nlohmann::json& j, const std::string& name) {

    auto context = j.at(name);

    std::string input;
    context.get_to(input);

    const auto& value = magic_enum::enum_cast<KeyCode>(input);
    if (not value.has_value()) {
        throw nlohmann::json::type_error::create(
                302, fmt::format("Expected a valid KeyCode in key '{}', but got '{}'", name, input), &context
        );
    }
    return value.value();
}

inline void from_json(const nlohmann::json& j, KeyboardControls& controls) {

    json::check_for_no_additional_keys(
            j, { "type", "rotate_left", "rotate_right", "move_left", "move_right", "move_down", "drop", "hold" }
    );

    controls.rotate_left = get_key_code_safe(j, "rotate_left");
    controls.rotate_right = get_key_code_safe(j, "rotate_right");
    controls.move_left = get_key_code_safe(j, "move_left");
    controls.move_right = get_key_code_safe(j, "move_right");
    controls.move_down = get_key_code_safe(j, "move_down");
    controls.drop = get_key_code_safe(j, "drop");
    controls.hold = get_key_code_safe(j, "hold");
    controls.validate();
}

using Controls = std::variant<KeyboardControls>;

inline void to_json(nlohmann::json& j, const Controls& controls) {
    std::visit(
            helper::overloaded{
                    [&](const KeyboardControls& keyboard_controls) {
                        to_json(j, keyboard_controls);
                        j["type"] = "keyboard";
                    },
            },
            controls
    );
}

inline void from_json(const nlohmann::json& j, Controls& controls) {
    const auto& type = j.at("type");

    if (type == "keyboard") {
        KeyboardControls keyboard_controls;
        from_json(j, keyboard_controls);
        controls = keyboard_controls;
    } else {
        throw std::runtime_error{ fmt::format("unsupported control type '{}'", to_string(type)) };
    }
}

struct Settings {
    std::array<Controls, max_num_players> controls;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Settings, controls)
