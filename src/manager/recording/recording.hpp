#pragma once


#include "additional_information.hpp"
#include "checksum_helper.hpp"
#include "helper/random.hpp"
#include "helper/types.hpp"
#include "manager/input_event.hpp"

#include <stdexcept>
#include <vector>

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
        AdditionalInformation information;

        TetrionHeader(Random::Seed seed, u32 starting_level, AdditionalInformation&& information);
    };

    struct Recording {
        constexpr static u32 magic_file_byte = 0x504F4FFF; // 0xFF and than OOP in ascii (in little endian)

    protected:
        std::vector<TetrionHeader> m_tetrion_headers;

        explicit Recording(std::vector<TetrionHeader>&& tetrion_headers)
            : m_tetrion_headers{ std::move(tetrion_headers) } { }

    public:
        Recording(const Recording&) = delete;
        Recording(Recording&&) = delete;
        Recording& operator=(const Recording&) = delete;
        Recording& operator=(Recording&&) = delete;
        virtual ~Recording() = default;

        [[nodiscard]] const std::vector<TetrionHeader>& tetrion_headers() const;

        [[nodiscard]] static Sha256Stream::Checksum
        get_header_checksum(u8 version_number, const std::vector<TetrionHeader>& tetrion_headers);
    };


} // namespace recorder
