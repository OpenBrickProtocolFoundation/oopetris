#pragma once

#include "controls.hpp"
#include "magic_enum_wrapper.hpp"
#include "recording.hpp"
#include <array>
#include <nlohmann/json.hpp>
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

inline void from_json(const nlohmann::json& j, KeyboardControls& controls) {
    std::string str;
    j.at("rotate_left").get_to(str);
    controls.rotate_left = magic_enum::enum_cast<KeyCode>(str).value();
    j.at("rotate_right").get_to(str);
    controls.rotate_right = magic_enum::enum_cast<KeyCode>(str).value();
    j.at("move_left").get_to(str);
    controls.move_left = magic_enum::enum_cast<KeyCode>(str).value();
    j.at("move_right").get_to(str);
    controls.move_right = magic_enum::enum_cast<KeyCode>(str).value();
    j.at("move_down").get_to(str);
    controls.move_down = magic_enum::enum_cast<KeyCode>(str).value();
    j.at("drop").get_to(str);
    controls.drop = magic_enum::enum_cast<KeyCode>(str).value();
    j.at("hold").get_to(str);
    controls.hold = magic_enum::enum_cast<KeyCode>(str).value();
}

using Controls = std::variant<KeyboardControls, ReplayControls>;

template<class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};
template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

inline void to_json(nlohmann::json& j, const Controls& controls) {
    std::visit(
            overloaded{ [&](const KeyboardControls& keyboard_controls) {
                           to_json(j, keyboard_controls);
                           j["type"] = "keyboard";
                       },
                        [&](const ReplayControls&) {
                            throw std::exception{}; // should never be serialized
                        } },
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
        throw std::exception{};
    }
}

struct Settings {
    std::array<Controls, max_num_players> controls;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Settings, controls)
