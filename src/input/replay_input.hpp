#pragma once

#include <recordings/utility/recording_reader.hpp>

#include "game_input.hpp"
#include "helper/windows.hpp"

#include <memory>

namespace input {


    struct ReplayGameInput : public GameInput {
    private:
        std::shared_ptr<recorder::RecordingReader> m_recording_reader;
        usize m_next_record_index{ 0 };
        usize m_next_snapshot_index{ 0 };
        const Input* m_underlying_input;

    public:
        OOPETRIS_GRAPHICS_EXPORTED
        ReplayGameInput(std::shared_ptr<recorder::RecordingReader> recording_reader, const Input* underlying_input);

        OOPETRIS_GRAPHICS_EXPORTED void update(SimulationStep simulation_step_index) override;
        OOPETRIS_GRAPHICS_EXPORTED void late_update(SimulationStep simulation_step_index) override;

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] std::optional<MenuEvent> get_menu_event(const SDL_Event& event
        ) const override;

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] std::string describe_menu_event(MenuEvent event) const override;

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] bool is_end_of_recording() const;

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] const Input* underlying_input() const override;
    };

} // namespace input
