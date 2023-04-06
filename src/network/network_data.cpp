

#include "network_data.hpp"
#include "network_transportable.hpp"
#include <cstdint>


InitializationData::InitializationData(InitializationDataType type, std::uint32_t uuid) : m_type{ type }, m_uuid{ uuid } {};


EventData::EventData(Input::Event event) : m_event{ event } {};
