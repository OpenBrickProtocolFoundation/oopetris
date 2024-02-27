#pragma once

#include "helper/clock_source.hpp"
#include "helper/optional.hpp"
#include "helper/random.hpp"
#include "helper/types.hpp"
#include "manager/controls.hpp"
#include "manager/event_listener.hpp"
#include "manager/input_event.hpp"
#include "manager/settings.hpp"

#include <functional>
#include <unordered_map>

struct Tetrion;

enum class InputCommand {
    MoveLeft,
    MoveRight,
    MoveDown,
    RotateLeft,
    RotateRight,
    Drop,
    Hold,
    ReleaseMoveDown,
};

enum class InputType { Touch, Keyboard, Controller, Recording };

struct Input {
public:
    using OnEventCallback = std::function<void(InputEvent)>;

private:
    enum class HoldableKey {
        Left,
        Right,
    };

    static constexpr u64 delayed_auto_shift_frames = 10;
    static constexpr u64 auto_repeat_rate_frames = 2;

    std::unordered_map<HoldableKey, u64> m_keys_hold;
    InputType m_input_type;

protected:
    Tetrion* m_target_tetrion{};
    OnEventCallback m_on_event_callback{};

    Input(InputType input_type) : m_input_type{ input_type } { }

    void handle_event(InputEvent event, SimulationStep simulation_step_index);

public:
    virtual void update(SimulationStep simulation_step_index);
    virtual void late_update(SimulationStep){};

    InputType input_type() const {
        return m_input_type;
    }

    bool supports_das() const {
        return m_input_type != InputType::Touch;
    }

    void set_target_tetrion(Tetrion* target_tetrion) {
        m_target_tetrion = target_tetrion;
    }

    void set_event_callback(OnEventCallback on_event_callback) {
        m_on_event_callback = on_event_callback;
    }

    Input(const Input&) = delete;
    Input& operator=(const Input&) = delete;

    virtual ~Input() = default;
};
