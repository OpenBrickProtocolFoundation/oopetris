
#include "sdl_key.hpp"
#include "helper/optional.hpp"
#include "helper/string_manipulation.hpp"
#include "helper/utils.hpp"

#include <algorithm>
#include <array>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <ranges>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>


sdl::Key::Key(SDL_KeyCode keycode, UnderlyingModifierType modifiers)
    : m_keycode{ keycode },
      m_modifiers{ modifiers } { }

sdl::Key::Key(SDL_KeyCode keycode, const std::vector<Modifier>& modifiers)
    : Key{ keycode, sdl::Key::sdl_modifier_from_modifiers(modifiers) } { }

sdl::Key::Key(const SDL_Keysym& keysym) : Key{ static_cast<SDL_KeyCode>(keysym.sym), keysym.mod } { }


[[nodiscard]] bool sdl::Key::is_key(const sdl::Key& other) const {
    return m_keycode == other.m_keycode;
}

namespace {
    constexpr SDL_Keymod to_sdl_modifier(sdl::Modifier modifier) {
        switch (modifier) {
            case sdl::Modifier::LSHIFT:
                return KMOD_LSHIFT;
            case sdl::Modifier::RSHIFT:
                return KMOD_RSHIFT;

            case sdl::Modifier::LCTRL:
                return KMOD_LCTRL;
            case sdl::Modifier::RCTRL:
                return KMOD_RCTRL;

            case sdl::Modifier::LALT:
                return KMOD_LALT;
            case sdl::Modifier::RALT:
                return KMOD_RALT;

            case sdl::Modifier::LGUI:
                return KMOD_LGUI;
            case sdl::Modifier::RGUI:
                return KMOD_RGUI;

            case sdl::Modifier::NUM:
                return KMOD_NUM;
            case sdl::Modifier::CAPS:
                return KMOD_CAPS;
            case sdl::Modifier::MODE:
                return KMOD_MODE;
            case sdl::Modifier::SCROLL:
                return KMOD_SCROLL;

            case sdl::Modifier::CTRL:
                return KMOD_CTRL;
            case sdl::Modifier::SHIFT:
                return KMOD_SHIFT;
            case sdl::Modifier::ALT:
                return KMOD_ALT;
            case sdl::Modifier::GUI:
                return KMOD_GUI;
            default:
                UNREACHABLE();
        }
    }

    [[maybe_unused]] constexpr sdl::Modifier from_sdl_modifier(SDL_Keymod modifier) {
        switch (modifier) {
            case KMOD_LSHIFT:
                return sdl::Modifier::LSHIFT;
            case KMOD_RSHIFT:
                return sdl::Modifier::RSHIFT;

            case KMOD_LCTRL:
                return sdl::Modifier::LCTRL;
            case KMOD_RCTRL:
                return sdl::Modifier::RCTRL;

            case KMOD_LALT:
                return sdl::Modifier::LALT;
            case KMOD_RALT:
                return sdl::Modifier::RALT;

            case KMOD_LGUI:
                return sdl::Modifier::LGUI;
            case KMOD_RGUI:
                return sdl::Modifier::RGUI;

            case KMOD_NUM:
                return sdl::Modifier::NUM;
            case KMOD_CAPS:
                return sdl::Modifier::CAPS;
            case KMOD_MODE:
                return sdl::Modifier::MODE;
            case KMOD_SCROLL:
                return sdl::Modifier::SCROLL;

            case KMOD_CTRL:
                return sdl::Modifier::CTRL;
            case KMOD_SHIFT:
                return sdl::Modifier::SHIFT;
            case KMOD_ALT:
                return sdl::Modifier::ALT;
            case KMOD_GUI:
                return sdl::Modifier::GUI;
            default:
                UNREACHABLE();
        }
    }

    constexpr std::tuple<std::array<sdl::Modifier, 8>, std::array<sdl::Modifier, 4>, std::array<sdl::Modifier, 4>>
    get_modifier_type_array() {
        return {
            {
             sdl::Modifier::LSHIFT,
             sdl::Modifier::RSHIFT,
             sdl::Modifier::LCTRL,
             sdl::Modifier::RCTRL,
             sdl::Modifier::LALT,
             sdl::Modifier::RALT,
             sdl::Modifier::LGUI,
             sdl::Modifier::RGUI,
             },
            {
             sdl::Modifier::NUM,
             sdl::Modifier::CAPS,
             sdl::Modifier::MODE,
             sdl::Modifier::SCROLL,
             },
            {
             sdl::Modifier::CTRL,
             sdl::Modifier::SHIFT,
             sdl::Modifier::ALT,
             sdl::Modifier::GUI,
             }
        };
    }

    [[nodiscard]] std::string sdl_key_name(SDL_KeyCode keycode) {
        const auto* name = SDL_GetKeyName(keycode);
        if (name == nullptr or std::strlen(name) == 0) {
            throw std::runtime_error(fmt::format(
                    "No name for the SDL key {}: {}", static_cast<std::underlying_type_t<decltype(keycode)>>(keycode),
                    SDL_GetError()
            ));
        }
        return std::string{ name };
    }


    [[maybe_unused]] sdl::ModifierType typeof_modifier(sdl::Modifier modifier) {
        const auto& [normal, special, multiple] = get_modifier_type_array();

        if (std::ranges::find(normal, modifier) != normal.cend()) {
            return sdl::ModifierType::Normal;
        }

        if (std::ranges::find(special, modifier) != special.cend()) {
            return sdl::ModifierType::Special;
        }

        if (std::ranges::find(multiple, modifier) != multiple.cend()) {
            return sdl::ModifierType::Multiple;
        }

        UNREACHABLE();
    }


    constexpr std::string modifier_to_string(sdl::Modifier modifier) {
        switch (modifier) {
            case sdl::Modifier::LSHIFT:
                return "Shift-L";
            case sdl::Modifier::RSHIFT:
                return "Shift-R";

            case sdl::Modifier::LCTRL:
                return "Ctrl-L";
            case sdl::Modifier::RCTRL:
                return "Ctrl-R";

            case sdl::Modifier::LALT:
                return "Alt-L";
            case sdl::Modifier::RALT:
                return "Alt-R";

            case sdl::Modifier::LGUI:
                return "Gui-L";
            case sdl::Modifier::RGUI:
                return "Gui-R";

            case sdl::Modifier::NUM:
                return "Num";
            case sdl::Modifier::CAPS:
                return "Caps";
            case sdl::Modifier::MODE:
                return "Mode";
            case sdl::Modifier::SCROLL:
                return "Scroll";

            case sdl::Modifier::CTRL:
                return "Ctrl";
            case sdl::Modifier::SHIFT:
                return "Shift";
            case sdl::Modifier::ALT:
                return "Alt";
            case sdl::Modifier::GUI:
                return "Gui";
            default:
                UNREACHABLE();
        }
    }


    helper::optional<sdl::Modifier> modifier_from_string(const std::string& modifier) {

        if (modifier.empty()) {
            return helper::nullopt;
        }

        const auto lower_case = string::to_lower_case(modifier);


        const std::unordered_map<std::string, sdl::Modifier> map{
            { "shift-l", sdl::Modifier::LSHIFT },
            { "shift-r", sdl::Modifier::RSHIFT },
            {  "ctrl-l",  sdl::Modifier::LCTRL },
            {  "ctrl-r",  sdl::Modifier::RCTRL },
            {   "alt-l",   sdl::Modifier::LALT },
            {   "alt-r",   sdl::Modifier::RALT },
            {   "gui-l",   sdl::Modifier::LGUI },
            {   "gui-r",   sdl::Modifier::RGUI },
            {     "num",    sdl::Modifier::NUM },
            {    "caps",   sdl::Modifier::CAPS },
            {    "mode",   sdl::Modifier::MODE },
            {  "scroll", sdl::Modifier::SCROLL },
            {    "ctrl",   sdl::Modifier::CTRL },
            {   "shift",  sdl::Modifier::SHIFT },
            {     "alt",    sdl::Modifier::ALT },
            {     "gui",    sdl::Modifier::GUI },
        };

        if (map.contains(lower_case)) {
            return map.at(lower_case);
        }

        return helper::nullopt;
    }

} // namespace


helper::expected<sdl::Key, std::string> sdl::Key::from_string(const std::string& value) {

    auto tokens = string::split_string_by_char(value, "+");
    for (auto& token : tokens) {
        string::trim(token);
    }

    std::vector<sdl::Modifier> modifiers{};

    for (size_t i = 0; i < tokens.size(); ++i) {
        const auto& token = tokens.at(i);

        if (token.empty()) {
            return helper::unexpected<std::string>{ "Empty token" };
        }

        if (i + 1 == tokens.size()) {
            const auto keycode = sdl::Key::sdl_keycode_from_string(token);
            if (not keycode.has_value()) {
                const auto modifier = modifier_from_string(token);
                if (modifier.has_value()) {
                    return helper::unexpected<std::string>{ "No key but only modifiers given" };
                }
                return helper::unexpected<std::string>{ keycode.error() };
            }

            //search for duplicates
            std::unordered_set<sdl::Modifier> values{};
            for (const auto& modifier : modifiers) {
                if (values.contains(modifier)) {
                    return helper::unexpected<std::string>{
                        fmt::format("Duplicate modifier: '{}'", modifier_to_string(modifier))
                    };
                }
                values.insert(modifier);
            }

            return sdl::Key{ keycode.value(), modifiers };
        }

        const auto modifier = modifier_from_string(token);
        if (not modifier.has_value()) {
            return helper::unexpected<std::string>{ fmt::format("Not a valid modifier: '{}'", token) };
        }

        modifiers.push_back(modifier.value());
    }

    return helper::unexpected<std::string>{ "empty input" };
}


[[nodiscard]] bool sdl::Key::has_modifier(const Modifier& modifier) const {
    const auto sdl_modifier = to_sdl_modifier(modifier);
    ;

    return (m_modifiers & sdl_modifier) != 0;
}

[[nodiscard]] bool sdl::Key::has_modifier_exact(const Modifier& modifier) const {

    sdl::Modifier has_not{};

    switch (modifier) {
        case sdl::Modifier::LSHIFT:
            has_not = sdl::Modifier::RSHIFT;
            break;
        case sdl::Modifier::RSHIFT:
            has_not = sdl::Modifier::LSHIFT;
            break;
        case sdl::Modifier::LCTRL:
            has_not = sdl::Modifier::RCTRL;
            break;
        case sdl::Modifier::RCTRL:
            has_not = sdl::Modifier::LCTRL;
            break;
        case sdl::Modifier::LALT:
            has_not = sdl::Modifier::RALT;
            break;
        case sdl::Modifier::RALT:
            has_not = sdl::Modifier::LALT;
            break;
        case sdl::Modifier::LGUI:
            has_not = sdl::Modifier::RGUI;
            break;
        case sdl::Modifier::RGUI:
            has_not = sdl::Modifier::LGUI;
            break;

        case sdl::Modifier::NUM:
        case sdl::Modifier::CAPS:
        case sdl::Modifier::MODE:
        case sdl::Modifier::SCROLL:

        case sdl::Modifier::CTRL:
        case sdl::Modifier::SHIFT:
        case sdl::Modifier::ALT:
        case sdl::Modifier::GUI: {
            const auto sdl_modifier = to_sdl_modifier(modifier);
            return (m_modifiers & sdl_modifier) == sdl_modifier;
        }

        default:
            UNREACHABLE();
    }


    const auto sdl_modifier = to_sdl_modifier(modifier);
    const auto sdl_modifier_not = to_sdl_modifier(has_not);

    return (m_modifiers & sdl_modifier) == sdl_modifier && (m_modifiers & sdl_modifier_not) == 0;
}


[[nodiscard]] bool sdl::Key::operator==(const Key& other) const {
    return is_equal(other, true);
}

[[nodiscard]] bool sdl::Key::is_equal(const Key& other, bool ignore_special_modifiers) const {
    if (not is_key(other)) {
        return false;
    }


    // fast path if they both are exactly the same
    if (other.m_modifiers == m_modifiers) {
        return true;
    }


    const auto& [_, special, multiple] = get_modifier_type_array();


    for (const auto& modifier : multiple) {
        const auto sdl_modifier = to_sdl_modifier(modifier);
        if (((other.m_modifiers & sdl_modifier) & (this->m_modifiers & sdl_modifier) //NOLINT(misc-redundant-expression)
            )
            == 0) {
            return false;
        }
    }

    if (ignore_special_modifiers) {
        return true;
    }

    return std::ranges::all_of(special, [this, &other](const auto& modifier) {
        const auto sdl_modifier = to_sdl_modifier(modifier);
        return ((other.m_modifiers & sdl_modifier) == (m_modifiers & sdl_modifier));
    });
}

[[nodiscard]] std::string sdl::Key::to_string() const {
    std::vector<std::string> parts{};

    const auto& [normal, special, multiple] = get_modifier_type_array();

    for (const auto modifier : special) {
        if (has_modifier(modifier)) {
            parts.emplace_back(modifier_to_string(modifier));
        }
    }

    for (const auto modifier : multiple) {
        if (has_modifier_exact(modifier)) {
            parts.emplace_back(modifier_to_string(modifier));
        }
    }

    for (const auto modifier : normal) {
        if (has_modifier_exact(modifier)) {
            parts.emplace_back(modifier_to_string(modifier));
        }
    }


    parts.emplace_back(sdl_key_name(m_keycode));

    return fmt::format("{}", fmt::join(parts, " + "));
}


[[nodiscard]] helper::expected<SDL_KeyCode, std::string> sdl::Key::sdl_keycode_from_string(const std::string& value) {
    const auto key = SDL_GetKeyFromName(value.c_str());
    if (key == SDLK_UNKNOWN) {
        return helper::unexpected<std::string>{
            fmt::format("No SDL key for the name '{}': {}", value, SDL_GetError())
        };
    }

    return static_cast<SDL_KeyCode>(key);
}

[[nodiscard]] sdl::Key::UnderlyingModifierType sdl::Key::sdl_modifier_from_modifiers(
        const std::vector<Modifier>& modifiers
) {
    UnderlyingModifierType result = KMOD_NONE;
    for (const auto& modifier : modifiers) {
        result |= to_sdl_modifier(modifier);
    }

    return result;
}
