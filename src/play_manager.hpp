

#pragma once

#include "game_manager.hpp"
#include <cstddef>
#include <memory>
#include <string>
#include <tl/optional.hpp>


/* abstract */ struct PlayManager {
public:
    explicit PlayManager();
    virtual ~PlayManager() = default;
    virtual std::size_t get_num_players() = 0;
    virtual tl::optional<std::string> init() = 0;
    virtual std::unique_ptr<Input>
    get_input(std::size_t index, GameManager* associated_game_manager, EventDispatcher* event_dispatcher) = 0;
};


struct SinglePlayer : public PlayManager {
public:
    explicit SinglePlayer();
    std::size_t get_num_players() override;
    tl::optional<std::string> init() override;
    std::unique_ptr<Input>
    get_input(std::size_t index, GameManager* associated_game_manager, EventDispatcher* event_dispatcher) override;
};
