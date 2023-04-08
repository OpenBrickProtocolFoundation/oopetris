#pragma once

#include "event_listener.hpp"
#include "input_event.hpp"
#include "network/connection_manager.hpp"
#include "random.hpp"
#include "settings.hpp"
#include "types.hpp"
#include <filesystem>
#include <fstream>
#include <functional>
#include <memory>
#include <tl/optional.hpp>
#include <unordered_map>

struct GameManager;

struct Input {
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
        Left = static_cast<int>(InputEvent::MoveLeft),
        Right = static_cast<int>(InputEvent::MoveRight),
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

struct ReplayInput : public Input {
private:
    Recording m_recording;
    usize m_next_record_index{ 0 };

public:
    explicit ReplayInput(GameManager* target_game_manager, Recording recording);

    void update() override;

private:
    [[nodiscard]] bool is_end_of_recording() const {
        return m_next_record_index >= m_recording.num_records();
    }
};

struct OnlineInput : public Input {
private:
    std::shared_ptr<Connection> m_connection;

public:
    explicit OnlineInput(GameManager* target_game_manager, std::shared_ptr<Connection> connection)
        : Input{ target_game_manager },
          m_connection{ connection } { }

    void update() override;
};