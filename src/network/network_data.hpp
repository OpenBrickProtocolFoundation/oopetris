

#pragma once

#include "../input.hpp"
#include "network_transportable.hpp"
#include <cstdint>

enum class InitializationDataType : uint8_t { Client, Server };

struct InitializationData : public Transportable {
private:
    InitializationDataType m_type;
    std::uint32_t m_uuid;

public:
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