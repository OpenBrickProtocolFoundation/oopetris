

#pragma once


#include "ai.hpp"
#include "input/input.hpp"

#include <core/helper/magic_enum_wrapper.hpp>

namespace {


    struct RadomInput : input::Input {

    public:
        OOPETRIS_GRAPHICS_EXPORTED RadomInput() : input::Input{ "random", input::InputType::AI } { }

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] std::optional<input::NavigationEvent>
        get_navigation_event(const SDL_Event&) const override {
            return std::nullopt;
        }

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] std::string describe_navigation_event(input::NavigationEvent
        ) const override {
            UNREACHABLE();
        }
    };


    struct RandomGameInput : public input::GameInput {
    private:
        Random m_random;
        std::optional<InputEvent> m_current;
        std::unique_ptr<RadomInput> m_input;

        InputEvent get_random_event() {

            int bound = magic_enum::enum_integer(InputEvent::RotateLeftReleased);

            return magic_enum::enum_cast<InputEvent>(m_random.random(bound)).value_or(InputEvent::RotateRightPressed);
        }

        InputEvent get_released_from_pressed(InputEvent start) {
            return magic_enum::enum_cast<InputEvent>(magic_enum::enum_underlying(start) + 7)
                    .value_or(InputEvent::RotateRightReleased);
        }

    public:
        OOPETRIS_GRAPHICS_EXPORTED
        RandomGameInput()
            : input::GameInput{ input::GameInputType::AI },
              m_random{ Random::generate_seed() },
              m_input{ std::make_unique<RadomInput>() } { }

        OOPETRIS_GRAPHICS_EXPORTED ~RandomGameInput() override { }

        RandomGameInput(const RandomGameInput& input) = delete;
        [[nodiscard]] RandomGameInput& operator=(const RandomGameInput& input) = delete;

        OOPETRIS_GRAPHICS_EXPORTED RandomGameInput(RandomGameInput&& input) noexcept = default;
        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] RandomGameInput& operator=(RandomGameInput&& input) noexcept = default;


        OOPETRIS_GRAPHICS_EXPORTED void update(SimulationStep simulation_step_index) override {

            std::optional<InputEvent> event = {};

            auto random_number = m_random.random();
            if (random_number >= 0.85) {
                if (m_current.has_value()) {
                    event = get_released_from_pressed(m_current.value());
                    m_current = std::nullopt;
                } else {
                    event = get_random_event();
                    m_current = event;
                }
            }

            if (event.has_value()) {
                GameInput::handle_event(event.value(), simulation_step_index);
            }
            GameInput::update(simulation_step_index);
        }

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] std::optional<input::MenuEvent> get_menu_event(const SDL_Event&)
                const override {
            return std::nullopt;
        }

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] std::string describe_menu_event(input::MenuEvent) const override {
            UNREACHABLE();
        }

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] const RadomInput* underlying_input() const override {
            return m_input.get();
        }
    };
} // namespace


namespace oopetris {


    struct DummyAI : AI {
    private:
        std::string m_name;

    public:
        DummyAI() : AI{ "dummy" } { }

        ~DummyAI() = default;


        std::shared_ptr<input::GameInput> input() override {
            return std::make_shared<RandomGameInput>();
        }
    };


} // namespace oopetris
