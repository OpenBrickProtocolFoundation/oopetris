#pragma once

#include <core/helper/expected.hpp>
#include <core/helper/types.hpp>

#include "helper/export_symbols.hpp"

#include <SDL.h>
#include <fmt/format.h>
#include <string>
#include <vector>

namespace sdl {

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

    enum class ModifierType : u8 { Normal, Multiple, Special };

    struct Key {
        // the difference between SDL_Keymod and ModifierType type is, that ModifierType is SDL_Keymod or-ed together, and supports arithmetic expressions out of the box (like & and |)
        using UnderlyingModifierType = std::underlying_type_t<SDL_Keymod>;

    private:
        SDL_KeyCode m_keycode;
        UnderlyingModifierType m_modifiers;

    public:
        OOPETRIS_GRAPHICS_EXPORTED explicit Key(SDL_KeyCode keycode, UnderlyingModifierType modifiers);
        OOPETRIS_GRAPHICS_EXPORTED explicit Key(SDL_KeyCode keycode, const std::vector<Modifier>& modifiers = {});
        OOPETRIS_GRAPHICS_EXPORTED explicit Key(const SDL_Keysym& keysym);

        OOPETRIS_GRAPHICS_EXPORTED static helper::expected<Key, std::string> from_string(const std::string& value);

        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED bool is_key(const Key& other) const;

        /**
        * @brief Checks if the key has a modifier, this performs a logical check, e.g. LALT and ALT are treated as match 
        * 
        * @param modifier 
        * @return bool 
        */
        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED bool has_modifier(const Modifier& modifier) const;

        /**
        * @brief Checks if the key has a modifier, this performs a exact check, e.g. LALT and ALT are treated as NON-match 
        * 
        * @param modifier 
        * @return bool 
        */
        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED bool has_modifier_exact(const Modifier& modifier) const;

        /**
 * @brief Shorthand for is_equal(key, true);
 * 
 * @param other 
 * @return bool 
 */
        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED bool operator==(const Key& other) const;

        /**
 * @brief compare two keys with another, this IS symmetrical. The result is true when these conditions are met:
 * 1: the key identifier are the same
 * 2: all "selected modifiers" have the same value, "same" means here, that either both have the modifier (as defined by has_modifier) or none of them has it
 * 3: selected modifiers => if ignore_special_modifiers is true, only the modifiers specified by the category "multiple" are selected. those are:
 * - sdl::Modifier::CTRL
 * - sdl::Modifier::SHIFT
 * - sdl::Modifier::ALT
 * - sdl::Modifier::GUI
 * 
 * if ignore_special_modifiers is false also the modifiers of teh category "special" are selected, these are:
 * - sdl::Modifier::NUM,
 * - sdl::Modifier::CAPS,
 * - sdl::Modifier::MODE,
 * - sdl::Modifier::SCROLL,
 * @param other 
 * @param ignore_special_modifiers 
 * @return bool 
 */
        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED bool is_equal(const Key& other, bool ignore_special_modifiers = true)
                const;

        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED std::string to_string() const;

    private:
        [[nodiscard]] static helper::expected<SDL_KeyCode, std::string> sdl_keycode_from_string(const std::string& value
        );

        [[nodiscard]] static UnderlyingModifierType sdl_modifier_from_modifiers(const std::vector<Modifier>& modifiers);
    };


} // namespace sdl

template<>
struct fmt::formatter<sdl::Key> : fmt::formatter<std::string> {
    auto format(const sdl::Key& key, format_context& ctx) const {
        return formatter<std::string>::format(key.to_string(), ctx);
    }
};


//TODO(Totto):  add input manager and rename curretn inputmanager to game_input manager or similar

// each devices can have multiple input devices, like keyboard, joycon etc. you can select mulltiple ones for navigation, some keys are ficed, like ctrl+c ctrl+v or arrow keys enter, esc tab,, some like wasd and similar can be chnaged by the inpout controller, support both click only and keyboard only, joyocn only, joyncon(s) + keyboard configurations


// each devices has  a name, e.g. keabord 1, keabyord 2, SDL_NumJoysticks

// SDL_JoystickNameForIndex
//SDL_JoystickName

//SDL_JoystickInstanceID

// if a type is disconnected, recognize that, and potentially pause the game !, show warning!

//SDL_JoystickCurrentPowerLevel
