#pragma once

#include "event_listener.hpp"
#include "settings.hpp"
#include "types.hpp"
#include <functional>
#include <tl/optional.hpp>
#include <unordered_map>

struct GameManager;

struct Input {
    enum class Event {
        RotateLeft,
        RotateRight,
        MoveLeft,
        MoveRight,
        MoveDown,
        Drop,
        ReleaseMoveDown,
    };

protected:
    GameManager* m_target_game_manager;

    explicit Input(GameManager* target_game_manager) : m_target_game_manager{ target_game_manager } { }

public:
    virtual void update() { }
    virtual ~Input() = default;
};

struct KeyboardInput : public Input, public EventListener {
private:
    enum class HoldableKey {
        Left = static_cast<int>(Event::MoveLeft),
        Right = static_cast<int>(Event::MoveRight),
    };

    static constexpr u64 delayed_auto_shift_frames = 10;
    static constexpr u64 auto_repeat_rate_frames = 2;

    std::unordered_map<HoldableKey, u64> m_keys_hold;
    KeyboardControls m_controls;

public:
    explicit KeyboardInput(GameManager* target_game_manager, KeyboardControls controls)
        : Input{ target_game_manager },
          m_controls{ controls } { }

    void update() override;

    void handle_event(const SDL_Event& event) override;
    void handle_keydown(const SDL_Event& event);
    void handle_keyup(const SDL_Event& event);
};
