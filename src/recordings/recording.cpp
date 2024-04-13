
#include "recording.hpp"
#include <stdexcept>


recorder::TetrionHeader::TetrionHeader(Random::Seed seed, u32 starting_level)
    : seed{ seed },
      starting_level{ starting_level } { }


[[nodiscard]] const std::vector<recorder::TetrionHeader>& recorder::Recording::tetrion_headers() const {
    return m_tetrion_headers;
}

[[nodiscard]] const recorder::AdditionalInformation& recorder::Recording::information() const {
    return m_information;
}


[[nodiscard]] Sha256Stream::Checksum recorder::Recording::get_header_checksum(
        u8 version_number,
        const std::vector<TetrionHeader>& tetrion_headers,
        const AdditionalInformation& information
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

    const auto information_checksum = information.get_checksum();
    if (not information_checksum.has_value()) {
        throw std::runtime_error(information_checksum.error());
    }

    sha256_creator << information_checksum.value();

    return sha256_creator.get_hash();
}
