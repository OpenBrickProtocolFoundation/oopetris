

#pragma once

#include "../input.hpp"
#include "../random.hpp"
#include "../types.hpp"
#include "network_transportable.hpp"
#include <cstdint>

enum class InitializationDataType : uint8_t { Receive, Send };

struct InitializationData : public Transportable {
public:
    static constexpr std::uint32_t serialUUID = 1;

    InitializationDataType type;
    std::uint32_t uuid;

    explicit InitializationData(InitializationDataType type, std::uint32_t uuid);
};

struct EventData : public Transportable {
    static constexpr std::uint32_t serialUUID = 2;

    InputEvent event;
    u64 simulation_step_index;

    explicit EventData(InputEvent event, u64 simulation_step_index);
};

struct ClientInitializationData : public Transportable {
public:
    std::uint32_t player_num;
    std::uint32_t your_player_id;
    Random::Seed seed;

    static constexpr std::uint32_t serialUUID = 3;
    explicit ClientInitializationData(std::uint32_t player_num, std::uint32_t your_player_id, Random::Seed seed);
};
