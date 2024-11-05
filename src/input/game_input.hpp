#pragma once

#include <core/helper/input_event.hpp>
#include <core/helper/random.hpp>
#include <core/helper/types.hpp>

#include <SDL.h>
#include <functional>
#include <unordered_map>

#include "helper/windows.hpp"

struct SimulatedTetrion;

namespace input {

    enum class GameInputCommand : u8 {
        MoveLeft,
        MoveRight,
        MoveDown,
        RotateLeft,
        RotateRight,
        Drop,
        Hold,
        ReleaseMoveDown,
    };

    enum class GameInputType : u8 { Touch, Keyboard, JoyStick, Controller, Recording, AI };

    enum class MenuEvent : u8 { OpenSettings, Pause };


    enum class HoldableKey : u8 {
        Left,
        Right,
    };

    // forward declaration
    struct Input;

    struct GameInput {
    public:
        using OnEventCallback = std::function<void(InputEvent, SimulationStep)>;

    private:
        static constexpr u64 delayed_auto_shift_frames = 10;
        static constexpr u64 auto_repeat_rate_frames = 2;

        std::unordered_map<HoldableKey, u64> m_keys_hold;
        GameInputType m_input_type;
        SimulatedTetrion* m_target_tetrion{};
        OnEventCallback m_on_event_callback;

    protected:
        explicit GameInput(GameInputType input_type) : m_input_type{ input_type } { }

        void handle_event(InputEvent event, SimulationStep simulation_step_index);

        [[nodiscard]] const SimulatedTetrion* target_tetrion() const {
            return m_target_tetrion;
        }

        [[nodiscard]] const OnEventCallback& on_event_callback() const {
            return m_on_event_callback;
        }

    public:
        OOPETRIS_GRAPHICS_EXPORTED GameInput(const GameInput&) = delete;
        OOPETRIS_GRAPHICS_EXPORTED GameInput& operator=(const GameInput&) = delete;

        OOPETRIS_GRAPHICS_EXPORTED GameInput(GameInput&&) = default;
        OOPETRIS_GRAPHICS_EXPORTED GameInput& operator=(GameInput&&) = default;

        OOPETRIS_GRAPHICS_EXPORTED virtual ~GameInput() = default;


        OOPETRIS_GRAPHICS_EXPORTED virtual void update(SimulationStep simulation_step_index);
        OOPETRIS_GRAPHICS_EXPORTED virtual void late_update(SimulationStep /*simulation_step*/){};

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] virtual std::optional<MenuEvent> get_menu_event(const SDL_Event& event
        ) const = 0;

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] virtual std::string describe_menu_event(MenuEvent event) const = 0;

        [[nodiscard]] GameInputType input_type() const {
            return m_input_type;
        }

        [[nodiscard]] bool supports_das() const {
            // todo support das with hold in touch mode
            return m_input_type != GameInputType::Touch;
        }

        void set_target_tetrion(SimulatedTetrion* target_tetrion) {
            m_target_tetrion = target_tetrion;
        }

        void set_event_callback(OnEventCallback on_event_callback) {
            m_on_event_callback = std::move(on_event_callback);
        }


        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] virtual const Input* underlying_input() const = 0;
    };
} // namespace input
