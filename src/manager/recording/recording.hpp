#pragma once


#include "helper/random.hpp"
#include "helper/types.hpp"
#include "manager/input_event.hpp"

#include <vector>

namespace recorder {

    struct RecordingError : public std::exception { };

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
    };

    struct Recording {
        constexpr static u32 magic_file_byte = 0xFF4F4F50; // 0xFF and than OOP in ascii

    protected:
        std::vector<TetrionHeader> m_tetrion_headers;

        Recording() = default;
        explicit Recording(std::vector<TetrionHeader> tetrion_headers)
            : m_tetrion_headers{ std::move(tetrion_headers) } { }

    public:
        Recording(const Recording&) = delete;
        Recording(Recording&&) = delete;
        Recording& operator=(const Recording&) = delete;
        Recording& operator=(Recording&&) = delete;
        virtual ~Recording() = default;

        [[nodiscard]] const std::vector<TetrionHeader>& tetrion_headers() const;
    };

} // namespace recorder
