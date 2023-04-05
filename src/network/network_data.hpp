

#pragma once

#include "../input.hpp"
#include "network_transportable.hpp"
#include <cstdint>

enum class InitializationDataType : uint8_t { Client, Server };

struct InitializationData : public Transportable {
private:
    InitializationDataType m_type;
    uint32_t m_uuid;

public:
    uint32_t serialUUID() const override;
    explicit InitializationData(InitializationDataType type, uint32_t uuid);
};

struct EventData : public Transportable {
private:
    Input::Event m_event;

public:
    uint32_t serialUUID() const override;
    explicit EventData(Input::Event event);
};