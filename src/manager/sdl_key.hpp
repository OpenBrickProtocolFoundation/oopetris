#pragma once

#include "helper/expected.hpp"

#include "helper/types.hpp"

#include <SDL.h>
#include <fmt/format.h>
#include <string>
#include <vector>

namespace SDL {

    enum class Modifier : u8 {
        LSHIFT,
        RSHIFT,

        LCTRL,
        RCTRL,

        LALT,
        RALT,

        LGUI,
        RGUI,

        NUM,
        CAPS,
        MODE,
        SCROLL,

        CTRL,
        SHIFT,
        ALT,
        GUI,
    };

    enum class ModifierType { Normal, Multiple, Special };

    struct Key {
        // the difference between SDL_Keymod and ModifierType type is, that ModifierType is SDL_Keymod or-ed together, and supports arithmetic expressions out of the box (like & and |)
        using UnderlyingModifierType = std::underlying_type_t<SDL_Keymod>;

    private:
        SDL_KeyCode m_keycode;
        UnderlyingModifierType m_modifiers;

    public:
        explicit Key(SDL_KeyCode keycode, UnderlyingModifierType modifiers);
        explicit Key(SDL_KeyCode keycode, const std::vector<Modifier>& modifiers = {});
        explicit Key(const SDL_Keysym& keysym);

        static helper::expected<Key, std::string> from_string(const std::string& value);

        [[nodiscard]] bool is_key(const Key& other) const;

        /**
        * @brief Checks if the key has a modifier, this performs a logical check, e.g. LALT and ALT are treated as match 
        * 
        * @param modifier 
        * @return bool 
        */
        [[nodiscard]] bool has_modifier(const Modifier& modifier) const;

        /**
        * @brief Checks if the key has a modifier, this performs a exact check, e.g. LALT and ALT are treated as NON-match 
        * 
        * @param modifier 
        * @return bool 
        */
        [[nodiscard]] bool has_modifier_exact(const Modifier& modifier) const;

        [[nodiscard]] bool operator==(const Key& other) const;

        [[nodiscard]] bool is_equal(const Key& other, bool ignore_special_modifiers = true) const;

        [[nodiscard]] std::string to_string() const;

    private:
        [[nodiscard]] static helper::expected<SDL_KeyCode, std::string> sdl_keycode_from_string(const std::string& value
        );

        [[nodiscard]] static UnderlyingModifierType sdl_modifier_from_modifiers(const std::vector<Modifier>& modifiers);
    };


} // namespace SDL

template<>
struct fmt::formatter<SDL::Key> : formatter<std::string> {
    auto format(const SDL::Key& key, format_context& ctx) {
        return formatter<std::string>::format(key.to_string(), ctx);
    }
};


//TODO: add input manager and rename curretn inputmanager to game_input manager or similar

// each devices can have multiple input devices, like keyboard, joycon etc. you can select mulltiple ones for navigation, some keys are ficed, like ctrl+c ctrl+v or arrow keys enter, esc tab,, some like wasd and similar can be chnaged by the inpout controller, support both click only and keyboard only, joyocn only, joyncon(s) + keyboard configurations


// each devices has  a name, e.g. keabord 1, keabyord 2, SDL_NumJoysticks

// SDL_JoystickNameForIndex
//SDL_JoystickName

//SDL_JoystickInstanceID

// if a type is disconnected, recognize that, and potentially pause the game !, show warning!

//SDL_JoystickCurrentPowerLevel
