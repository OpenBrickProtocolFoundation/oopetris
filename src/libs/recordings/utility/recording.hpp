#pragma once

#include "./export_symbols.hpp"
#include <core/helper/input_event.hpp>
#include <core/helper/random.hpp>
#include <core/helper/types.hpp>

#include "./additional_information.hpp"
#include "./checksum_helper.hpp"

#include <stdexcept>
#include <vector>


namespace constants::recording {

    constexpr const char* extension = "rec";
    constexpr static u32 magic_file_byte = 0x504F4FFF; // 0xFF and than OOP in ascii (in little endian)

} // namespace constants::recording


namespace recorder {

    struct Record final {
        u8 tetrion_index;
        u64 simulation_step_index;
        InputEvent event;
    };

    enum class MagicByte : u8 {
        Record = 42,
        Snapshot = 43,
    };

    struct TetrionHeader final {
        Random::Seed seed;
        u32 starting_level;

        OOPETRIS_RECORDINGS_EXPORTED TetrionHeader(Random::Seed seed, u32 starting_level);
    };

    struct Recording {
    protected:
        std::vector<TetrionHeader> m_tetrion_headers;
        AdditionalInformation m_information;

        explicit Recording(std::vector<TetrionHeader>&& tetrion_headers, AdditionalInformation&& information)
            : m_tetrion_headers{ std::move(tetrion_headers) },
              m_information{ std::move(information) } { }

    public:
        constexpr const static u8 current_supported_version_number = 1;

        Recording(const Recording&) = delete;
        Recording(Recording&&) = delete;
        Recording& operator=(const Recording&) = delete;
        Recording& operator=(Recording&&) = delete;
        virtual ~Recording() = default;

        [[nodiscard]] OOPETRIS_RECORDINGS_EXPORTED const std::vector<TetrionHeader>& tetrion_headers() const;

        [[nodiscard]] OOPETRIS_RECORDINGS_EXPORTED const AdditionalInformation& information() const;

        [[nodiscard]] OOPETRIS_RECORDINGS_EXPORTED static Sha256Stream::Checksum get_header_checksum(
                u8 version_number,
                const std::vector<TetrionHeader>& tetrion_headers,
                const AdditionalInformation& information
        );
    };


} // namespace recorder
