

#include "play_manager.hpp"
#include "game_manager.hpp"
#include <cstddef>
#include <exception>

PlayManager::PlayManager(){};

SinglePlayer::SinglePlayer() : PlayManager{} {};

std::size_t SinglePlayer::get_num_players() {
    return 1;
};

tl::optional<std::string> SinglePlayer::init() {
    return {};
}

std::unique_ptr<Input>
SinglePlayer::get_input(std::size_t index, GameManager* associated_game_manager, EventDispatcher event_dispatcher) {

    if (index != 0) {
        throw std::range_error{ "SinglePlayer mode: error in index of get_input" };
    }

    auto keyboard_input = std::make_unique<KeyboardInput>(associated_game_manager);
    event_dispatcher.register_listener(keyboard_input.get());
    return keyboard_input;
}
