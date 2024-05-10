
#include "sdl_key.hpp"
#include "helper/optional.hpp"
#include "helper/utils.hpp"

#include <array>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>


SDL::Key::Key(SDL_KeyCode keycode, UnderlyingModifierType modifiers)
    : m_keycode{ keycode },
      m_modifiers{ modifiers } { }

SDL::Key::Key(SDL_KeyCode keycode, const std::vector<Modifier>& modifiers)
    : Key{ keycode, SDL::Key::sdl_modifier_from_modifiers(modifiers) } { }

SDL::Key::Key(const SDL_Keysym& keysym) : Key{ static_cast<const SDL_KeyCode>(keysym.sym), keysym.mod } { }


[[nodiscard]] bool SDL::Key::is_key(const SDL::Key& other) const {
    return m_keycode == other.m_keycode;
}

namespace {
    constexpr SDL_Keymod to_sdl_modifier(SDL::Modifier modifier) {
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

    [[maybe_unused]] constexpr SDL::Modifier from_sdl_modifier(SDL_Keymod modifier) {
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

    constexpr std::tuple<std::array<SDL::Modifier, 8>, std::array<SDL::Modifier, 4>, std::array<SDL::Modifier, 4>>
    get_modifier_type_array() {
        return {
            {
             SDL::Modifier::LSHIFT,
             SDL::Modifier::RSHIFT,
             SDL::Modifier::LCTRL,
             SDL::Modifier::RCTRL,
             SDL::Modifier::LALT,
             SDL::Modifier::RALT,
             SDL::Modifier::LGUI,
             SDL::Modifier::RGUI,
             },
            {
             SDL::Modifier::NUM,
             SDL::Modifier::CAPS,
             SDL::Modifier::MODE,
             SDL::Modifier::SCROLL,
             },
            {
             SDL::Modifier::CTRL,
             SDL::Modifier::SHIFT,
             SDL::Modifier::ALT,
             SDL::Modifier::GUI,
             }
        };
    }

    [[nodiscard]] std::string sdl_key_name(SDL_KeyCode keycode) {
        const auto* name = SDL_GetKeyName(keycode);
        if (std::strlen(name) == 0) {
            throw std::runtime_error(fmt::format(
                    "No name for the sdl key {}: {}", static_cast<std::underlying_type_t<decltype(keycode)>>(keycode),
                    SDL_GetError()
            ));
        }
        return std::string{ name };
    }


    [[maybe_unused]] SDL::ModifierType typeof_modifier(SDL::Modifier modifier) {
        const auto& [normal, special, multiple] = get_modifier_type_array();

        if (std::find(normal.cbegin(), normal.cend(), modifier) != normal.cend()) {
            return SDL::ModifierType::Normal;
        }

        if (std::find(special.cbegin(), special.cend(), modifier) != special.cend()) {
            return SDL::ModifierType::Special;
        }

        if (std::find(multiple.cbegin(), multiple.cend(), modifier) != multiple.cend()) {
            return SDL::ModifierType::Multiple;
        }

        utils::unreachable();
    }


    constexpr std::string modifier_to_string(SDL::Modifier modifier) {
        switch (modifier) {
            case SDL::Modifier::LSHIFT:
                return "Shift-L";
            case SDL::Modifier::RSHIFT:
                return "Shift-R";

            case SDL::Modifier::LCTRL:
                return "Ctrl-L";
            case SDL::Modifier::RCTRL:
                return "Ctrl-R";

            case SDL::Modifier::LALT:
                return "Alt-L";
            case SDL::Modifier::RALT:
                return "Alt-R";

            case SDL::Modifier::LGUI:
                return "Gui-L";
            case SDL::Modifier::RGUI:
                return "Gui-R";

            case SDL::Modifier::NUM:
                return "Num";
            case SDL::Modifier::CAPS:
                return "Caps";
            case SDL::Modifier::MODE:
                return "Mode";
            case SDL::Modifier::SCROLL:
                return "Scroll";

            case SDL::Modifier::CTRL:
                return "Ctrl";
            case SDL::Modifier::SHIFT:
                return "Shift";
            case SDL::Modifier::ALT:
                return "Alt";
            case SDL::Modifier::GUI:
                return "Gui";
            default:
                utils::unreachable();
        }
    }

    std::string to_lower_case(const std::string& input) {
        auto result = input;
        for (size_t i = 0; i < result.size(); ++i) {
            auto& elem = result.at(i);
            elem = std::tolower(elem);
        }

        return result;
    }

    // for string delimiter
    std::vector<std::string> split_string_by_char(const std::string& start, const std::string& delimiter) {
        size_t pos_start = 0;
        size_t pos_end;
        const auto delim_len = delimiter.length();

        std::vector<std::string> res{};

        while ((pos_end = start.find(delimiter, pos_start)) != std::string::npos) {
            auto token = start.substr(pos_start, pos_end - pos_start);
            pos_start = pos_end + delim_len;
            res.push_back(token);
        }

        res.push_back(start.substr(pos_start));
        return res;
    }

    // trim from start (in place)
    inline void ltrim(std::string& s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !std::isspace(ch); }));
    }

    // trim from end (in place)
    inline void rtrim(std::string& s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), s.end());
    }

    void trim(std::string& s) {
        ltrim(s);
        rtrim(s);
    }

    constexpr helper::optional<SDL::Modifier> modifier_from_string(std::string modifier) {

        if (modifier.empty()) {
            return helper::nullopt;
        }

        const auto lower_case = to_lower_case(modifier);


        const std::unordered_map<std::string, SDL::Modifier> map{
            { "shift-l", SDL::Modifier::LSHIFT },
            { "shift-r", SDL::Modifier::RSHIFT },
            {  "ctrl-l",  SDL::Modifier::LCTRL },
            {  "ctrl-r",  SDL::Modifier::RCTRL },
            {   "alt-l",   SDL::Modifier::LALT },
            {   "alt-r",   SDL::Modifier::RALT },
            {   "gui-l",   SDL::Modifier::LGUI },
            {   "gui-r",   SDL::Modifier::RGUI },
            {     "num",    SDL::Modifier::NUM },
            {    "caps",   SDL::Modifier::CAPS },
            {    "mode",   SDL::Modifier::MODE },
            {  "scroll", SDL::Modifier::SCROLL },
            {    "ctrl",   SDL::Modifier::CTRL },
            {   "shift",  SDL::Modifier::SHIFT },
            {     "alt",    SDL::Modifier::ALT },
            {     "gui",    SDL::Modifier::GUI },
        }; // namespace

        if (map.contains(lower_case)) {
            return map.at(lower_case);
        }

        return helper::nullopt;
    }

} // namespace


helper::expected<SDL::Key, std::string> SDL::Key::from_string(const std::string& value) {


    auto tokens = split_string_by_char(value, "+");
    for (auto& token : tokens) {
        trim(token);
    }

    std::vector<SDL::Modifier> modifiers{};

    for (size_t i = 0; i < tokens.size(); ++i) {
        const auto& token = tokens.at(i);
        if (i + 1 == tokens.size()) {
            const auto keycode = SDL::Key::sdl_keycode_from_string(token);
            if (not keycode.has_value()) {
                return helper::unexpected<std::string>{ keycode.error() };
            }

            //search for duplicates
            std::unordered_set<SDL::Modifier> values{};
            for (const auto& modifier : modifiers) {
                if (values.contains(modifier)) {
                    return helper::unexpected<std::string>{
                        fmt::format("Duplicate modifier: '{}'", modifier_to_string(modifier))
                    };
                }
                values.insert(modifier);
            }

            return SDL::Key{ keycode.value(), modifiers };
        }

        const auto modifier = modifier_from_string(token);
        if (not modifier.has_value()) {
            return helper::unexpected<std::string>{ fmt::format("Not a valid modifier: '{}'", token) };
        }

        modifiers.push_back(modifier.value());
    }

    return helper::unexpected<std::string>{ "empty input" };
}


[[nodiscard]] bool SDL::Key::has_modifier(const Modifier& modifier) const {
    const auto sdl_modifier = to_sdl_modifier(modifier);
    ;

    return (m_modifiers & sdl_modifier) != 0;
}

[[nodiscard]] bool SDL::Key::has_modifier_exact(const Modifier& modifier) const {
    const auto sdl_modifier = to_sdl_modifier(modifier);

    return (m_modifiers & sdl_modifier) == sdl_modifier;
}


[[nodiscard]] bool SDL::Key::operator==(const Key& other) const {
    return is_equal(other, true);
}

[[nodiscard]] bool SDL::Key::is_equal(const Key& other, bool ignore_special_modifiers) const {
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
        if (((other.m_modifiers & sdl_modifier) & (m_modifiers & sdl_modifier)) == 0) {
            return false;
        }
    }

    if (ignore_special_modifiers) {
        return true;
    }

    for (const auto& modifier : special) {
        const auto sdl_modifier = to_sdl_modifier(modifier);
        if ((other.m_modifiers & sdl_modifier) != (m_modifiers & sdl_modifier)) {
            return false;
        }
    }

    return true;
}

[[nodiscard]] std::string SDL::Key::to_string() const {
    std::vector<std::string> parts{};

    const auto& [_, special, multiple] = get_modifier_type_array();

    for (const auto modifier : special) {
        if (has_modifier(modifier)) {
            parts.emplace_back(modifier_to_string(modifier));
        }
    }

    parts.emplace_back(sdl_key_name(m_keycode));

    return fmt::format("{}", fmt::join(parts, " + "));
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

[[nodiscard]] SDL::Key::UnderlyingModifierType SDL::Key::sdl_modifier_from_modifiers(
        const std::vector<Modifier>& modifiers
) {
    UnderlyingModifierType result = KMOD_NONE;
    for (const auto& modifier : modifiers) {
        result |= to_sdl_modifier(modifier);
    }

    return result;
}
