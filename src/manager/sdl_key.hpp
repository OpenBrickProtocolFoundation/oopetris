#pragma once

#include "helper/expected.hpp"

#include "helper/types.hpp"

#include <SDL.h>
#include <string>
#include <utility>
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

    struct Key {
        // the difference between SDL_Keymod and ModifierType type is, that ModifierType is SDL_Keymod or-ed together, and supports arithmetic expressions out of teh box (like & and |)
        using ModifierType = std::underlying_type_t<SDL_Keymod>;

    private:
        SDL_KeyCode m_keycode;
        ModifierType m_modifiers;

    public:
        explicit Key(SDL_KeyCode keycode, ModifierType modifiers);
        explicit Key(SDL_KeyCode keycode, const std::vector<Modifier>& modifiers = {});
        explicit Key(const SDL_Keysym& keysym);

        static helper::expected<Key, std::string>
        from_string(const std::string& value, const std::vector<Modifier>& modifiers = {});


        [[nodiscard]] bool is_key(const Key& other) const;

        [[nodiscard]] bool has_modifier(const Modifier& modifier) const;

        [[nodiscard]] bool operator==(const Key& other) const;

        [[nodiscard]] std::string name() const;

    private:
        [[nodiscard]] static helper::expected<SDL_KeyCode, std::string> sdl_keycode_from_string(const std::string& value
        );

        [[nodiscard]] static ModifierType sdl_modifier_from_modifiers(const std::vector<Modifier>& modifiers);
    };


} // namespace SDL


namespace detail {

    struct ModifierIterator {
    public:
        using ContentType = std::pair<bool, SDL::Modifier>;
        using Container = std::vector<ContentType>;
        using iterator = Container::iterator;
        using const_iterator = Container::const_iterator;

    private:
        std::vector<ContentType> m_underlying_container{};

    public:
        ModifierIterator(SDL::Key::ModifierType modifiers);

        [[nodiscard]] const_iterator begin() const;
        [[nodiscard]] iterator begin();

        [[nodiscard]] const_iterator end() const;
        [[nodiscard]] iterator end();
    };
} // namespace detail


//TODO: add input manager and rename curretn inputmanager to game_input manager or similar

// each devices can have multiple input devices, like keyboard, joycon etc. you can select mulltiple ones for navigation, some keys are ficed, like ctrl+c ctrl+v or arrow keys enter, esc tab,, some like wasd and similar can be chnaged by the inpout controller, support both click only and keyboard only, joyocn only, joyncon(s) + keyboard configurations


// each devices has  a name, e.g. keabord 1, keabyord 2, SDL_NumJoysticks

// SDL_JoystickNameForIndex
//SDL_JoystickName

//SDL_JoystickInstanceID

// if a type is disconnected, recognize that, and potentially pause the game !, show warning!

//SDL_JoystickCurrentPowerLevel
