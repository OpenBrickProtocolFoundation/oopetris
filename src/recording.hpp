#pragma once

#include "input_event.hpp"
#include "random.hpp"
#include "types.hpp"
#include <filesystem>
#include <fstream>
#include <vector>

struct LoadRecordingError : public std::exception { };

struct Recording final {
private:
    struct Record {
        usize simulation_step_index;
        InputEvent event;
        Record(usize simulation_step_index, InputEvent event)
            : simulation_step_index{ simulation_step_index },
              event{ event } { }
    };

    Random::Seed m_seed;
    std::vector<Record> m_records;

public:
    explicit Recording(const Random::Seed seed) : m_seed{ seed } { }

    explicit Recording(const std::filesystem::path& path) : m_seed{ 0 } {
        auto file = std::ifstream{ path, std::ios::in | std::ios::binary };
        if (not file) {
            throw LoadRecordingError{};
        }

        // read seed
        file.read(reinterpret_cast<char*>(&m_seed), sizeof(m_seed));
        if (not file) {
            throw LoadRecordingError{};
        }

        // read records
        while (file) {
            usize simulation_step_index;
            std::underlying_type_t<InputEvent> event;
            file.read(reinterpret_cast<char*>(&simulation_step_index), sizeof(simulation_step_index));
            file.read(reinterpret_cast<char*>(&event), sizeof(event));
            // todo: validation
            add_record(simulation_step_index, static_cast<InputEvent>(event));
        }
    }

    [[nodiscard]] Random::Seed seed() const {
        return m_seed;
    }

    void add_record(const usize simulation_step_index, const InputEvent event) {
        m_records.emplace_back(simulation_step_index, event);
    }

    [[nodiscard]] const Record& at(const usize index) const {
        return m_records.at(index);
    }

    [[nodiscard]] usize num_records() const {
        return m_records.size();
    }

    [[nodiscard]] auto begin() const {
        return m_records.cbegin();
    }

    [[nodiscard]] auto end() const {
        return m_records.cend();
    }
};
