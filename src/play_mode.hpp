

#pragma once

#include "application.hpp"
#include "input.hpp"
#include "random.hpp"
#include "settings.hpp"
#include "types.hpp"
#include <cstddef>
#include <memory>
#include <string>
#include <tl/expected.hpp>
#include <utility>
#include <vector>
struct StartState {
    std::size_t num_players;
    Random::Seed seed;
};


//TODO: use m_is_replay_mode to not make network connection or keyboardinput!!

/* abstract */ struct PlayMode {
private:
    Settings m_settings;
    bool m_is_replay_mode;

public:
    explicit PlayMode(const bool is_replay_mode);
    virtual ~PlayMode() = default;
    virtual tl::expected<StartState, std::string> init(Settings settings, Random::Seed seed);
    virtual std::unique_ptr<Input>
    get_input(u8 index, Tetrion* tetrion, Input::OnEventCallback event_callback, EventDispatcher* event_dispatcher) = 0;
    Settings settings();
};


struct SinglePlayer : public PlayMode {
public:
    explicit SinglePlayer(const bool is_replay_mode);
    tl::expected<StartState, std::string> init(Settings settings, Random::Seed seed) override;
    std::unique_ptr<Input> get_input(
            u8 index,
            Tetrion* tetrion,
            Input::OnEventCallback event_callback,
            EventDispatcher* event_dispatcher
    ) override;
};