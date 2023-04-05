

#include "network_data.hpp"
#include "network_transportable.hpp"
#include <cstdint>

std::uint32_t InitializationData::serialUUID() const {
    return 1;
}

InitializationData::InitializationData(InitializationDataType type, uint32_t uuid) : m_type{ type }, m_uuid{ uuid } {};


std::uint32_t EventData::serialUUID() const {
    return 2;
}

EventData::EventData(Input::Event event) : m_event{ event } {};
