#pragma once

#include "event_listener.hpp"
#include <functional>
#include <tl/optional.hpp>

struct GameManager;

struct Input {
    enum class Event {
        RotateLeft,
        RotateRight,
        MoveLeft,
        MoveRight,
        MoveDown,
        Drop,
    };

protected:
    GameManager* m_target_game_manager;

    explicit Input(GameManager* target_game_manager) : m_target_game_manager{ target_game_manager } { }

public:
    virtual ~Input() = default;
};

struct KeyboardInput : public Input, public EventListener {
    explicit KeyboardInput(GameManager* target_game_manager) : Input{ target_game_manager } { }

    void handle_event(const SDL_Event& event) override;
};
