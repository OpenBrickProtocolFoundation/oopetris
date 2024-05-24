

#pragma once


#include "game_input.hpp"
#include "graphics/point.hpp"
#include "graphics/rect.hpp"
#include "graphics/window.hpp"
#include "helper/bool_wrapper.hpp"
#include "helper/expected.hpp"
#include "helper/optional.hpp"
#include "manager/service_provider.hpp"


#include <fmt/format.h>
#include <memory>
#include <vector>

namespace input {

    enum class InputType : u8 { Keyboard, Pointer, JoyStick, Controller };

    enum class NavigationEvent : u8 { OK, DOWN, UP, LEFT, RIGHT, BACK, TAB };

    enum class SpecialRequest : u8 { WindowFocusLost, InputsChanged };


    struct Input {
    private:
        std::string m_name;
        InputType m_type;

    public:
        Input(std::string name, InputType type);

        virtual ~Input();

        Input(const Input& input) noexcept;
        Input& operator=(const Input& input) noexcept;

        Input(Input&& input) noexcept;
        Input& operator=(Input&& input) noexcept;

        [[nodiscard]] const std::string& name() const;
        [[nodiscard]] InputType type() const;

        [[nodiscard]] virtual helper::optional<NavigationEvent> get_navigation_event(const SDL_Event& event) const = 0;

        [[nodiscard]] virtual std::string describe_navigation_event(NavigationEvent event) const = 0;
    };

    enum class PointerEvent : u8 { Motion, PointerDown, PointerUp };

    struct PointerEventHelper {
    private:
        shapes::IPoint m_pos;
        PointerEvent m_event;

    public:
        PointerEventHelper(shapes::IPoint pos, PointerEvent event);

        [[nodiscard]] PointerEvent event() const;

        [[nodiscard]] shapes::IPoint position() const;

        [[nodiscard]] bool is_in(const shapes::URect& rect) const;

        [[nodiscard]] bool is_in(const shapes::IRect& rect) const;

        [[nodiscard]] bool operator==(PointerEvent event) const;
    };


    struct PointerInput : Input {
        explicit PointerInput(const std::string& name);

        [[nodiscard]] virtual helper::optional<PointerEventHelper> get_pointer_event(const SDL_Event& event) const = 0;

        [[nodiscard]] virtual SDL_Event offset_pointer_event(const SDL_Event& event, const shapes::IPoint& point)
                const = 0;
    };

    struct InputManager {
    private:
        std::vector<std::unique_ptr<Input>> m_inputs;

    public:
        explicit InputManager(const std::shared_ptr<Window>& window);

        [[nodiscard]] const std::vector<std::unique_ptr<Input>>& inputs() const;

        [[nodiscard]] helper::optional<NavigationEvent> get_navigation_event(const SDL_Event& event) const;

        [[nodiscard]] helper::optional<PointerEventHelper> get_pointer_event(const SDL_Event& event) const;

        /**
        * @brief Offsets a pointer event, only safe to call, if get_pointer_event returns a non null optional
        * 
        * @param event the original SDL Event
        * @param point the point to offset it by
        * @return SDL_Event which has the correct offset
        */
        [[nodiscard]] SDL_Event offset_pointer_event(const SDL_Event& event, const shapes::IPoint& point) const;

        [[nodiscard]] helper::BoolWrapper<SpecialRequest> process_special_inputs(const SDL_Event& event);

        [[nodiscard]] helper::expected<std::shared_ptr<input::GameInput>, std::string> get_game_input(
                ServiceProvider* service_provider
        );

        [[nodiscard]] const input::Input* get_primary_input();
    };


    struct InputSettings {

        template<typename T>
        [[nodiscard]] static helper::expected<bool, std::string> has_unique_members(const std::vector<T>& to_check) {
            std::vector<T> already_bound{};


            for (const auto& single_check : to_check) {

                if (std::ranges::find(already_bound, single_check) != already_bound.cend()) {
                    return helper::unexpected<std::string>{ fmt::format("KeyCode already bound: '{}'", single_check) };
                }

                already_bound.push_back(single_check);
            }

            return true;
        }
    };


} // namespace input
