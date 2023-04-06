#pragma once

#include "event_listener.hpp"
#include "settings.hpp"
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

    static constexpr double auto_shift_delay = 1.0 / 6.0;
    static constexpr double auto_repeat_rate = 1.0 / 30.0;

    std::unordered_map<HoldableKey, double> m_keys_hold;
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
