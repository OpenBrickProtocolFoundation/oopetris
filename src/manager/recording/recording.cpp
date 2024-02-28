
#include "recording.hpp"


[[nodiscard]] const std::vector<recorder::TetrionHeader>& recorder::Recording::tetrion_headers() const {
    return m_tetrion_headers;
}

[[nodiscard]] Sha256Stream::Checksum recorder::Recording::get_header_checksum(
        u8 version_number,
        const std::vector<recorder::TetrionHeader>& tetrion_headers
) {
    Sha256Stream sha256_creator{};

    static_assert(sizeof(version_number) == 1);
    sha256_creator << version_number;

    sha256_creator << static_cast<u8>(tetrion_headers.size());

    for (const auto& header : tetrion_headers) {
        static_assert(sizeof(decltype(header.seed)) == 8);
        sha256_creator << header.seed;
        static_assert(sizeof(decltype(header.starting_level)) == 4);
        sha256_creator << header.starting_level;
    }

    return sha256_creator.get_hash();
}
