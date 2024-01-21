#pragma once

#include "capabilities.hpp"
#include "key_codes.hpp"
#include "magic_enum_wrapper.hpp"

struct KeyboardControls final {
    KeyCode rotate_left = KeyCode::Left;
    KeyCode rotate_right = KeyCode::Right;
    KeyCode move_left = KeyCode::A;
    KeyCode move_right = KeyCode::D;
    KeyCode move_down = KeyCode::S;
    KeyCode drop = KeyCode::W;
    KeyCode hold = KeyCode::Tab;

    void validate() const {
        std::vector<KeyCode> already_bound_keycodes{};
        if (utils::device_supports_keys()) {
            for (const auto& key : utils::get_bound_keys()) {
                const auto key_code = from_sdl_keycode(static_cast<SDL_KeyCode>(key));
                if (key_code == KeyCode::Unknown) {
                    throw std::runtime_error("Couldn't map bound key '" + std::to_string(key) + "'");
                }

                already_bound_keycodes.push_back(key_code);
            }
        }


        const std::vector<KeyCode> to_use{ rotate_left, rotate_right, move_left, move_right, move_down, drop, hold };


        for (const auto key_to_use : to_use) {

            if (std::find(already_bound_keycodes.cbegin(), already_bound_keycodes.cend(), key_to_use)
                != already_bound_keycodes.cend()) {
                std::string error_code = "KeyCode already bound: '";
                error_code += magic_enum::enum_name(key_to_use);
                error_code += "'";

                throw std::runtime_error(error_code);
            }

            already_bound_keycodes.push_back(key_to_use);
        }
    }
};
