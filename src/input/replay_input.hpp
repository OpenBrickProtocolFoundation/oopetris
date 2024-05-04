#pragma once

#include "game_input.hpp"
#include "recordings/recording_reader.hpp"

#include <memory>

namespace input {


    struct ReplayGameInput : public GameInput {
    private:
        std::shared_ptr<recorder::RecordingReader> m_recording_reader;
        usize m_next_record_index{ 0 };
        usize m_next_snapshot_index{ 0 };

    public:
        ReplayGameInput(std::shared_ptr<recorder::RecordingReader> recording_reader);

        void update(SimulationStep simulation_step_index) override;
        void late_update(SimulationStep simulation_step_index) override;

        [[nodiscard]] helper::optional<MenuEvent> get_menu_event(const SDL_Event& event) const override;

        [[nodiscard]] std::string describe_menu_event(MenuEvent event) const override;

        [[nodiscard]] bool is_end_of_recording() const;
    };

} // namespace input
