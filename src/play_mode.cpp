#include "play_mode.hpp"
#include "random.hpp"
#include "tetrion.hpp"
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <tl/expected.hpp>
#include <utility>

PlayMode::PlayMode(const bool is_replay_mode) : m_is_replay_mode{ is_replay_mode } {};

tl::expected<StartState, std::string> PlayMode::init(Settings settings, Random::Seed seed) {
    m_settings = settings;
    return StartState{ 0, seed };
};

Settings PlayMode::settings() {
    return m_settings;
}

SinglePlayer::SinglePlayer(const bool is_replay_mode) : PlayMode{ is_replay_mode } {};


tl::expected<StartState, std::string> SinglePlayer::init(Settings settings, Random::Seed seed) {
    PlayMode::init(settings, seed);
    return StartState{ 1, seed };
}

std::unique_ptr<Input> SinglePlayer::get_input(
        u8 index,
        Tetrion* tetrion,
        Input::OnEventCallback event_callback,
        EventDispatcher* event_dispatcher
) {

    if (index != 0) {
        throw std::range_error{ "SinglePlayer mode: error in index of get_input" };
    }

    tetrion->set_player_num(0);

    return utils::create_input(settings().controls.at(index), tetrion, std::move(event_callback), event_dispatcher);
}
