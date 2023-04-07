

#include "play_manager.hpp"
#include "game_manager.hpp"
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <tl/expected.hpp>
#include <utility>

PlayManager::PlayManager(){};

SinglePlayer::SinglePlayer() : PlayManager{} {};


tl::expected<StartState, std::string> SinglePlayer::init() {
    return StartState{ 1 };
}

std::pair<std::size_t, std::unique_ptr<Input>>
SinglePlayer::get_input(std::size_t index, GameManager* associated_game_manager, EventDispatcher* event_dispatcher) {

    if (index != 0) {
        throw std::range_error{ "SinglePlayer mode: error in index of get_input" };
    }

    auto keyboard_input = std::make_unique<KeyboardInput>(associated_game_manager);
    event_dispatcher->register_listener(keyboard_input.get());
    return std::pair<std::size_t, std::unique_ptr<Input>>{ 0, std::move(keyboard_input) };
}
