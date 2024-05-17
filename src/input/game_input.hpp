#pragma once

#include "helper/clock_source.hpp"
#include "helper/optional.hpp"
#include "helper/random.hpp"
#include "helper/types.hpp"
#include "manager/event_listener.hpp"
#include "manager/input_event.hpp"

#include <functional>
#include <unordered_map>

struct Tetrion;

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

    enum class GameInputType : u8 { Touch, Keyboard, Controller, Recording };

    enum class MenuEvent : u8 { OpenSettings, Pause };


    struct GameInput {
    public:
        using OnEventCallback = std::function<void(InputEvent, SimulationStep)>;

    private:
        enum class HoldableKey : u8 {
            Left,
            Right,
        };

        static constexpr u64 delayed_auto_shift_frames = 10;
        static constexpr u64 auto_repeat_rate_frames = 2;

        std::unordered_map<HoldableKey, u64> m_keys_hold;
        GameInputType m_input_type;
        Tetrion* m_target_tetrion{};
        OnEventCallback m_on_event_callback;

    protected:
        explicit GameInput(GameInputType input_type) : m_input_type{ input_type } { }

        void handle_event(InputEvent event, SimulationStep simulation_step_index);

        [[nodiscard]] const Tetrion* target_tetrion() const {
            return m_target_tetrion;
        }

        [[nodiscard]] const OnEventCallback& on_event_callback() const {
            return m_on_event_callback;
        }

    public:
        virtual void update(SimulationStep simulation_step_index);
        virtual void late_update(SimulationStep /*simulation_step*/){};

        [[nodiscard]] virtual helper::optional<MenuEvent> get_menu_event(const SDL_Event& event) const = 0;

        [[nodiscard]] virtual std::string describe_menu_event(MenuEvent event) const = 0;

        [[nodiscard]] GameInputType input_type() const {
            return m_input_type;
        }

        [[nodiscard]] bool supports_das() const {
            // todo support das with hold in touch mode
            return m_input_type != GameInputType::Touch;
        }

        void set_target_tetrion(Tetrion* target_tetrion) {
            m_target_tetrion = target_tetrion;
        }

        void set_event_callback(OnEventCallback on_event_callback) {
            m_on_event_callback = std::move(on_event_callback);
        }

        GameInput(const GameInput&) = delete;
        GameInput& operator=(const GameInput&) = delete;

        GameInput(GameInput&&) = default;
        GameInput& operator=(GameInput&&) = default;

        virtual ~GameInput() = default;
    };
} // namespace input
