#pragma once

#include "helper/clock_source.hpp"
#include "helper/optional.hpp"
#include "helper/random.hpp"
#include "helper/types.hpp"
#include "input/controls.hpp"
#include "input/event_listener.hpp"
#include "input/input_event.hpp"
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

protected:
    Tetrion* m_target_tetrion;
    OnEventCallback m_on_event_callback;

    Input(Tetrion* target_tetrion, OnEventCallback on_event_callback = OnEventCallback{})
        : m_target_tetrion{ target_tetrion },
          m_on_event_callback{ std::move(on_event_callback) } { }

    void handle_event(InputEvent event, SimulationStep simulation_step_index);

public:
    virtual void update(SimulationStep simulation_step_index);
    virtual void late_update(SimulationStep){};

    Input(const Input&) = delete;
    Input& operator=(const Input&) = delete;

    virtual ~Input() = default;
};

struct EventDispatcher;

struct KeyboardInput : public Input, public EventListener {
private:
    KeyboardControls m_controls;
    std::vector<SDL_Event> m_event_buffer;
    EventDispatcher* m_event_dispatcher;

public:
    KeyboardInput(
            Tetrion* target_tetrion,
            KeyboardControls controls,
            EventDispatcher* event_dispatcher,
            OnEventCallback on_event_callback = OnEventCallback{}
    );

    ~KeyboardInput() override;

    void handle_event(const SDL_Event& event, const Window* window) override;

    void update(SimulationStep simulation_step_index) override;

private:
    [[nodiscard]] helper::optional<InputEvent> sdl_event_to_input_event(const SDL_Event& event) const;
};

struct RecordingReader;

struct ReplayInput : public Input {
private:
    RecordingReader* m_recording_reader;
    usize m_next_record_index{ 0 };
    usize m_next_snapshot_index{ 0 };

public:
    ReplayInput(Tetrion* target_tetrion, OnEventCallback on_event_callback, RecordingReader* recording_reader);

    void update(SimulationStep simulation_step_index) override;
    void late_update(SimulationStep simulation_step_index) override;

private:
    [[nodiscard]] bool is_end_of_recording() const;
};

#if defined(__ANDROID__)
struct TouchInput final : public Input, public EventListener {
private:
    struct PressedState {
        Uint32 timestamp;
        float x;
        float y;
        explicit PressedState(Uint32 timestamp, float x, float y) : timestamp{ timestamp }, x{ x }, y{ y } { }
    };

    std::unordered_map<SDL_FingerID, helper::optional<PressedState>> m_finger_state;
    std::vector<SDL_Event> m_event_buffer;


public:
    explicit TouchInput(Tetrion* target_tetrion, OnEventCallback on_event_callback = OnEventCallback{})
        : Input{ target_tetrion, std::move(on_event_callback) } { }

    void handle_event(const SDL_Event& event, const Window* window) override;
    void update(SimulationStep simulation_step_index) override;

private:
    [[nodiscard]] helper::optional<InputEvent> sdl_event_to_input_event(const SDL_Event& event);
};
#endif

#if defined(__SWITCH__)
struct JoystickInput : public Input, public EventListener {
private:
    std::vector<SDL_Event> m_event_buffer;

public:
    JoystickInput(Tetrion* target_tetrion, OnEventCallback on_event_callback = OnEventCallback{})
        : Input{ target_tetrion, std::move(on_event_callback) } { }

    void handle_event(const SDL_Event& event, const Window* window) override;

    void update(SimulationStep simulation_step_index) override;

private:
    [[nodiscard]] helper::optional<InputEvent> sdl_event_to_input_event(const SDL_Event& event) const;
};
#endif
