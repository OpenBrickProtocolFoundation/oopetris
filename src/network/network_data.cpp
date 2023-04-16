

#include "network_data.hpp"
#include "../random.hpp"
#include "network_transportable.hpp"
#include <cstdint>


InitializationData::InitializationData(InitializationDataType type, std::uint32_t uuid) : type{ type }, uuid{ uuid } {};


EventData::EventData(InputEvent event, u64 simulation_step_index)
    : event{ event },
      simulation_step_index{ simulation_step_index } {};


ClientInitializationData::ClientInitializationData(
        std::uint32_t player_num,
        std::uint32_t your_player_id,
        Random::Seed seed
)
    : player_num{ player_num },
      your_player_id{ your_player_id },
      seed{ seed } {};
