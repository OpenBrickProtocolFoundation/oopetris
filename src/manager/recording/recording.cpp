
#include "recording.hpp"


[[nodiscard]] const std::vector<recorder::TetrionHeader>& recorder::Recording::tetrion_headers() const {
    return m_tetrion_headers;
}
