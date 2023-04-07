

#pragma once

#include "../input.hpp"
#include "network_transportable.hpp"
#include <cstdint>

enum class InitializationDataType : uint8_t { Receive, Send };

struct InitializationData : public Transportable {
public:
    InitializationDataType m_type;
    std::uint32_t m_uuid;

    static constexpr std::uint32_t serialUUID = 1;
    explicit InitializationData(InitializationDataType type, std::uint32_t uuid);
};

struct EventData : public Transportable {
private:
    Input::Event m_event;

public:
    static constexpr std::uint32_t serialUUID = 2;
    explicit EventData(Input::Event event);
    Input::Event event() const;
};

struct ClientInitializationData : public Transportable {
public:
    std::uint32_t player_num;
    std::uint32_t your_player_id;

    static constexpr std::uint32_t serialUUID = 3;
    explicit ClientInitializationData(std::uint32_t player_num, std::uint32_t your_player_id);
};