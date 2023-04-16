#pragma once

#include "controls.hpp"
#include "event_dispatcher.hpp"
#include "event_listener.hpp"
#include "input_event.hpp"
#include "network/connection_manager.hpp"
#include "network/network_data.hpp"
#include "random.hpp"
#include "settings.hpp"
#include "types.hpp"
#include <filesystem>
#include <functional>
#include <memory>
#include <tl/optional.hpp>
#include <unordered_map>
#include <vector>

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

protected:
    explicit Input(Tetrion* target_tetrion) : m_target_tetrion{ target_tetrion } { }
    explicit Input(Tetrion* target_tetrion, OnEventCallback on_event_callback)
        : m_target_tetrion{ target_tetrion },
          m_on_event_callback{ std::move(on_event_callback) } { }
    void handle_event(InputEvent event);

public:
    virtual void update();
    virtual void late_update(){};
    virtual ~Input() = default;
};

struct KeyboardInput : public Input, public EventListener {
private:
    KeyboardControls m_controls;

public:
    explicit KeyboardInput(Tetrion* target_tetrion, KeyboardControls controls)
        : Input{ target_tetrion },
          m_controls{ controls } { }

    explicit KeyboardInput(Tetrion* target_tetrion, OnEventCallback on_event_callback, KeyboardControls controls)
        : Input{ target_tetrion, std::move(on_event_callback) },
          m_controls{ controls } { }

    void handle_event(const SDL_Event& event) override;

private:
    [[nodiscard]] tl::optional<InputEvent> sdl_event_to_input_event(const SDL_Event& event) const;
};

struct RecordingReader;

struct ReplayInput : public Input {
private:
    u8 m_tetrion_index;
    RecordingReader* m_recording_reader;
    usize m_next_record_index{ 0 };
    usize m_next_snapshot_index{ 0 };

public:
    ReplayInput(
            Tetrion* target_tetrion,
            u8 tetrion_index,
            OnEventCallback on_event_callback,
            RecordingReader* recording_reader
    );

    void update() override;
    void late_update() override;

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

    std::unordered_map<SDL_FingerID, tl::optional<PressedState>> m_finger_state;


public:
    explicit TouchInput(Tetrion* target_tetrion, OnEventCallback on_event_callback = OnEventCallback{})
        : Input{ target_tetrion, std::move(on_event_callback) } { }

    void handle_event(const SDL_Event& event) override;

private:
    [[nodiscard]] tl::optional<InputEvent> sdl_event_to_input_event(const SDL_Event& event);
};
#endif


namespace utils {
    [[nodiscard]] std::unique_ptr<Input> create_input(
            Controls controls,
            Tetrion* associated_tetrion,
            Input::OnEventCallback on_event_callback,
            EventDispatcher* event_dispatcher
    );
}