

#pragma once

#include <core/helper/bool_wrapper.hpp>
#include <core/helper/expected.hpp>
#include <core/helper/point.hpp>

#include "game_input.hpp"
#include "graphics/rect.hpp"
#include "graphics/window.hpp"
#include "helper/export_symbols.hpp"
#include "manager/service_provider.hpp"


#include <fmt/base.h>
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
        OOPETRIS_GRAPHICS_EXPORTED Input(std::string name, InputType type);

        OOPETRIS_GRAPHICS_EXPORTED virtual ~Input();

        OOPETRIS_GRAPHICS_EXPORTED Input(const Input& input) noexcept;
        OOPETRIS_GRAPHICS_EXPORTED Input& operator=(const Input& input) noexcept;

        OOPETRIS_GRAPHICS_EXPORTED Input(Input&& input) noexcept;
        OOPETRIS_GRAPHICS_EXPORTED Input& operator=(Input&& input) noexcept;

        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED const std::string& name() const;
        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED InputType type() const;

        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED virtual std::optional<NavigationEvent> get_navigation_event(
                const SDL_Event& event
        ) const = 0;

        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED virtual std::string describe_navigation_event(NavigationEvent event
        ) const = 0;
    };

    enum class PointerEvent : u8 { Motion, PointerDown, PointerUp, Wheel };

    struct PointerEventHelper {
    private:
        shapes::IPoint m_pos;
        PointerEvent m_event;

    public:
        OOPETRIS_GRAPHICS_EXPORTED PointerEventHelper(shapes::IPoint pos, PointerEvent event);

        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED PointerEvent event() const;

        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED shapes::IPoint position() const;

        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED bool is_in(const shapes::URect& rect) const;

        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED bool is_in(const shapes::IRect& rect) const;

        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED bool operator==(PointerEvent event) const;
    };


    struct PointerInput : Input {
        OOPETRIS_GRAPHICS_EXPORTED explicit PointerInput(const std::string& name);

        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED virtual std::optional<PointerEventHelper> get_pointer_event(
                const SDL_Event& event
        ) const = 0;

        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED virtual SDL_Event
        offset_pointer_event(const SDL_Event& event, const shapes::IPoint& point) const = 0;
    };

    struct InputManager {
    private:
        std::vector<std::unique_ptr<Input>> m_inputs;

    public:
        OOPETRIS_GRAPHICS_EXPORTED explicit InputManager(const std::shared_ptr<Window>& window);

        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED const std::vector<std::unique_ptr<Input>>& inputs() const;

        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED std::optional<NavigationEvent> get_navigation_event(
                const SDL_Event& event
        ) const;

        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED std::optional<PointerEventHelper> get_pointer_event(
                const SDL_Event& event
        ) const;

        /**
        * @brief Offsets a pointer event, only safe to call, if get_pointer_event returns a non null optional
        * 
        * @param event the original SDL Event
        * @param point the point to offset it by
        * @return SDL_Event which has the correct offset
        */
        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED SDL_Event
        offset_pointer_event(const SDL_Event& event, const shapes::IPoint& point) const;

        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED helper::BoolWrapper<SpecialRequest> process_special_inputs(
                const SDL_Event& event
        );

        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED helper::expected<std::shared_ptr<input::GameInput>, std::string>
        get_game_input(ServiceProvider* service_provider);

        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED const input::Input* get_primary_input();
    };


    struct InputSettings {

        template<typename T>
            requires fmt::formattable<T>
        [[nodiscard]] static helper::expected<void, std::string> has_unique_members(const std::vector<T>& to_check) {
            std::vector<T> already_bound{};


            for (const auto& single_check : to_check) {

                if (std::ranges::find(already_bound, single_check) != already_bound.cend()) {
                    return helper::unexpected<std::string>{ fmt::format("KeyCode already bound: '{}'", single_check) };
                }

                already_bound.push_back(single_check);
            }

            return {};
        }
    };


} // namespace input
