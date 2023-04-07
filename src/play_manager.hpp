

#pragma once

#include "game_manager.hpp"
#include <cstddef>
#include <memory>
#include <string>
#include <tl/expected.hpp>
#include <utility>
#include <vector>

struct StartState {
    std::size_t num_players;
    // std::vector<void> state;
    //TODO add seed here
};


/* abstract */ struct PlayManager {
public:
    explicit PlayManager();
    virtual ~PlayManager() = default;
    virtual tl::expected<StartState, std::string> init() = 0;
    virtual std::pair<std::size_t, std::unique_ptr<Input>>
    get_input(std::size_t index, GameManager* associated_game_manager, EventDispatcher* event_dispatcher) = 0;
};


struct SinglePlayer : public PlayManager {
public:
    explicit SinglePlayer();
    tl::expected<StartState, std::string> init() override;
    std::pair<std::size_t, std::unique_ptr<Input>>
    get_input(std::size_t index, GameManager* associated_game_manager, EventDispatcher* event_dispatcher) override;
};
