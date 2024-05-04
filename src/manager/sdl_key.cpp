
#include "sdl_key.hpp"
#include "helper/utils.hpp"

#include <fmt/format.h>
#include <tuple>
#include <type_traits>


SDL::Key::Key(SDL_KeyCode keycode, ModifierType modifiers) : m_keycode{ keycode }, m_modifiers{ modifiers } { }

SDL::Key::Key(SDL_KeyCode keycode, const std::vector<Modifier>& modifiers)
    : Key{ keycode, SDL::Key::sdl_modifier_from_modifiers(modifiers) } { }

SDL::Key::Key(const SDL_Keysym& keysym) : Key{ static_cast<const SDL_KeyCode>(keysym.sym), keysym.mod } { }

helper::expected<SDL::Key, std::string>
SDL::Key::from_string(const std::string& value, const std::vector<SDL::Modifier>& modifiers) {

    const auto keycode = SDL::Key::sdl_keycode_from_string(value);
    if (not keycode.has_value()) {
        return helper::unexpected<std::string>{ keycode.error() };
    }

    const auto raw_modifiers = SDL::Key::sdl_modifier_from_modifiers(modifiers);
    return SDL::Key{ keycode.value(), raw_modifiers };
}


[[nodiscard]] bool SDL::Key::is_key(const SDL::Key& other) const {
    return m_keycode == other.m_keycode;
}

namespace {
    SDL_Keymod to_sdl_modifier(SDL::Modifier modifier) {
        switch (modifier) {
            case SDL::Modifier::LSHIFT:
                return KMOD_LSHIFT;
            case SDL::Modifier::RSHIFT:
                return KMOD_RSHIFT;

            case SDL::Modifier::LCTRL:
                return KMOD_LCTRL;
            case SDL::Modifier::RCTRL:
                return KMOD_RCTRL;

            case SDL::Modifier::LALT:
                return KMOD_LALT;
            case SDL::Modifier::RALT:
                return KMOD_RALT;

            case SDL::Modifier::LGUI:
                return KMOD_LGUI;
            case SDL::Modifier::RGUI:
                return KMOD_RGUI;

            case SDL::Modifier::NUM:
                return KMOD_NUM;
            case SDL::Modifier::CAPS:
                return KMOD_CAPS;
            case SDL::Modifier::MODE:
                return KMOD_MODE;
            case SDL::Modifier::SCROLL:
                return KMOD_SCROLL;

            case SDL::Modifier::CTRL:
                return KMOD_CTRL;
            case SDL::Modifier::SHIFT:
                return KMOD_SHIFT;
            case SDL::Modifier::ALT:
                return KMOD_ALT;
            case SDL::Modifier::GUI:
                return KMOD_GUI;
            default:
                utils::unreachable();
        }
    }

    SDL::Modifier from_sdl_modifier(SDL_Keymod modifier) {
        switch (modifier) {
            case KMOD_LSHIFT:
                return SDL::Modifier::LSHIFT;
            case KMOD_RSHIFT:
                return SDL::Modifier::RSHIFT;

            case KMOD_LCTRL:
                return SDL::Modifier::LCTRL;
            case KMOD_RCTRL:
                return SDL::Modifier::RCTRL;

            case KMOD_LALT:
                return SDL::Modifier::LALT;
            case KMOD_RALT:
                return SDL::Modifier::RALT;

            case KMOD_LGUI:
                return SDL::Modifier::LGUI;
            case KMOD_RGUI:
                return SDL::Modifier::RGUI;

            case KMOD_NUM:
                return SDL::Modifier::NUM;
            case KMOD_CAPS:
                return SDL::Modifier::CAPS;
            case KMOD_MODE:
                return SDL::Modifier::MODE;
            case KMOD_SCROLL:
                return SDL::Modifier::SCROLL;

            case KMOD_CTRL:
                return SDL::Modifier::CTRL;
            case KMOD_SHIFT:
                return SDL::Modifier::SHIFT;
            case KMOD_ALT:
                return SDL::Modifier::ALT;
            case KMOD_GUI:
                return SDL::Modifier::GUI;
            default:
                utils::unreachable();
        }
    }
} // namespace

[[nodiscard]] bool SDL::Key::has_modifier(const Modifier& modifier) const {
    const auto sdl_modifier = to_sdl_modifier(modifier);
    return (m_modifiers & sdl_modifier) != KMOD_NONE;
}

[[nodiscard]] bool SDL::Key::operator==(const Key& other) const {
    if (not is_key(other)) {
        return false;
    }


    // fast path, if the second one has no modifiers
    if (other.m_modifiers == KMOD_NONE) {
        return m_modifiers == KMOD_NONE;
    }

    //TODO: use a feaster method, this takes a long time!
    const auto it = detail::ModifierIterator(other.m_modifiers);

    for (const auto& [present, modifier] : it) {
        if (present != not has_modifier(modifier)) {
            return false;
        }
    }

    return true;
}

[[nodiscard]] std::string SDL::Key::name() const {
    const auto* name = SDL_GetKeyName(m_keycode);
    if (std::strlen(name) == 0) {
        throw std::runtime_error(fmt::format(
                "No name for the sdl key {}: {}", static_cast<std::underlying_type_t<decltype(m_keycode)>>(m_keycode),
                SDL_GetError()
        ));
    }
    return std::string{ name };
}


[[nodiscard]] helper::expected<SDL_KeyCode, std::string> SDL::Key::sdl_keycode_from_string(const std::string& value) {
    const auto key = SDL_GetKeyFromName(value.c_str());
    if (key == SDLK_UNKNOWN) {
        return helper::unexpected<std::string>{
            fmt::format("No sdl key for the name '{}': {}", value, SDL_GetError())
        };
    }

    return static_cast<const SDL_KeyCode>(key);
}

[[nodiscard]] SDL::Key::ModifierType SDL::Key::sdl_modifier_from_modifiers(const std::vector<Modifier>& modifiers) {
    ModifierType result = KMOD_NONE;
    for (const auto& modifier : modifiers) {
        result |= to_sdl_modifier(modifier);
    }

    return result;
}


detail::ModifierIterator::ModifierIterator(SDL::Key::ModifierType modifiers) {

    const std::vector<std::tuple<SDL_Keymod, SDL_Keymod, SDL_Keymod>> triples{
        { KMOD_LSHIFT, KMOD_RSHIFT, KMOD_SHIFT },
        {  KMOD_LCTRL,  KMOD_RCTRL,  KMOD_CTRL },
        {   KMOD_LALT,   KMOD_RALT,   KMOD_ALT },
        {   KMOD_LGUI,   KMOD_RGUI,   KMOD_GUI },
    };


    const std::vector<SDL_Keymod> normal_mods{
        KMOD_NUM,
        KMOD_CAPS,
        KMOD_MODE,
        KMOD_SCROLL,
    };

    for (const auto& [left_mod, right_mod, both_mod] : triples) {

        std::tuple<bool, bool, bool> result = { false, false, false };

        if ((modifiers & both_mod) == both_mod) {
            result = { true, true, true };
        } else if ((modifiers & left_mod) == left_mod) {
            result = { true, false, true };
        } else if ((modifiers & right_mod) == right_mod) {
            result = { false, true, true };
        }

        m_underlying_container.emplace_back(std::get<0>(result), from_sdl_modifier(left_mod));
        m_underlying_container.emplace_back(std::get<1>(result), from_sdl_modifier(right_mod));
        m_underlying_container.emplace_back(std::get<2>(result), from_sdl_modifier(both_mod));
    }

    for (const auto& mod : normal_mods) {
        m_underlying_container.emplace_back((modifiers & mod) != KMOD_NONE, from_sdl_modifier(mod));
    }
}


[[nodiscard]] detail::ModifierIterator::const_iterator detail::ModifierIterator::begin() const {
    return m_underlying_container.begin();
}

[[nodiscard]] detail::ModifierIterator::iterator detail::ModifierIterator::begin() {
    return m_underlying_container.begin();
}

[[nodiscard]] detail::ModifierIterator::const_iterator detail::ModifierIterator::end() const {
    return m_underlying_container.end();
}

[[nodiscard]] detail::ModifierIterator::iterator detail::ModifierIterator::end() {
    return m_underlying_container.end();
}
