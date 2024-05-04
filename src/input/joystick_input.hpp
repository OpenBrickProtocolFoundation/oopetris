#pragma once


#include "SDL_joystick.h"
#include "helper/expected.hpp"
#include "input.hpp"
#include "input/game_input.hpp"
#include "manager/event_dispatcher.hpp"

#include <fmt/format.h>

namespace joystick {
    struct GUID {
    private:
        std::array<u8, 16> m_guid;

    public:
        GUID();
        GUID(const SDL_GUID& data);

        [[nodiscard]] bool operator==(const GUID& other) const;

        [[nodiscard]] operator std::string() const;
    };
} // namespace joystick


template<>
struct fmt::formatter<joystick::GUID> : formatter<std::string> {
    auto format(const joystick::GUID& guid, format_context& ctx) {
        return formatter<std::string>::format(std::string{ guid }, ctx);
    }
};

namespace input {

    struct JoystickInput : Input {
    private:
        SDL_Joystick* m_joystick;
        SDL_JoystickID m_instance_id;

        [[nodiscard]] static helper::optional<std::unique_ptr<JoystickInput>>
        get_joystick_by_guid(const joystick::GUID& guid, SDL_Joystick* joystick, SDL_JoystickID instance_id);

    public:
        JoystickInput(SDL_Joystick* joystick, SDL_JoystickID instance_id, const std::string& name);
        virtual ~JoystickInput();

        [[nodiscard]] static helper::expected<std::unique_ptr<JoystickInput>, std::string> get_by_device_index(
                int device_index
        );
        [[nodiscard]] SDL_JoystickID instance_id() const;
    };


    //TODO: also support gamecontroller API
    // see: https://github.com/mdqinc/SDL_GameControllerDB?tab=readme-ov-file

    struct JoyStickInputManager {
    private:
        std::vector<std::unique_ptr<JoystickInput>> m_inputs{};

    public:
        explicit JoyStickInputManager();
        [[nodiscard]] const std::vector<std::unique_ptr<JoystickInput>>& inputs() const;

        [[nodiscard]] helper::optional<NavigationEvent> get_navigation_event(const SDL_Event& event) const;

        [[nodiscard]] helper::optional<PointerEventHelper> get_pointer_event(const SDL_Event& event) const;

        [[nodiscard]] bool process_special_inputs(const SDL_Event& event);
    };


    //TODO: differntiate different controllers and modes, e.g the switch can have pro controller, the included ones, each of them seperate etc.

#if defined(__CONSOLE__)
#if defined(__SWITCH__)
    struct SwitchJoystickInput_Type1 : JoystickInput {

        //TODO
        static joystick::GUID guid{};

    public:
        SwitchJoystickInput_Type1(SDL_Joystick* joystick, SDL_JoystickID instance_id, const std::string& name);

        [[nodiscard]] helper::optional<NavigationEvent> get_navigation_event(const SDL_Event& event) const override;
    };


#elif defined(__3DS__)

    struct _3DSJoystickInput_Type1 : JoystickInput {

        //TODO
        static joystick::GUID guid{};

    public:
        _3DSJoystickInput_Type1(SDL_Joystick* joystick, SDL_JoystickID instance_id, const std::string& name);

        [[nodiscard]] helper::optional<NavigationEvent> get_navigation_event(const SDL_Event& event) const override;
    };


#endif

#endif

    struct JoystickGameInput : public GameInput, public EventListener {
    private:
        std::vector<SDL_Event> m_event_buffer;
        EventDispatcher* m_event_dispatcher;

    public:
        JoystickGameInput(EventDispatcher* event_dispatcher)
            : GameInput{ GameInputType::Controller },
              m_event_dispatcher{ event_dispatcher } {
            m_event_dispatcher->register_listener(this);
        }

        ~JoystickGameInput() override {
            m_event_dispatcher->unregister_listener(this);
        }

        void handle_event(const SDL_Event& event) override;

        void update(SimulationStep simulation_step_index) override;

    private:
        [[nodiscard]] helper::optional<InputEvent> sdl_event_to_input_event(const SDL_Event& event) const;
    };

} // namespace input


//TODO:
/* 
#elif defined(__SWITCH__)
    switch (action) {
        case CrossPlatformAction::OK:
            return "A";
        case CrossPlatformAction::PAUSE:
        case CrossPlatformAction::UNPAUSE:
            return "PLUS";
        case CrossPlatformAction::CLOSE:
        case CrossPlatformAction::EXIT:
            return "MINUS";
        case CrossPlatformAction::DOWN:
            return "Down";
        case CrossPlatformAction::UP:
            return "Up";
        case CrossPlatformAction::LEFT:
            return "Left";
        case CrossPlatformAction::RIGHT:
            return "Right";
        case CrossPlatformAction::OPEN_SETTINGS:
            return "Y";
        default:
            utils::unreachable();
    }

#elif defined(__3DS__)
    switch (action) {
        case CrossPlatformAction::OK:
            return "A";
        case CrossPlatformAction::PAUSE:
        case CrossPlatformAction::UNPAUSE:
            return "Y";
        case CrossPlatformAction::CLOSE:
        case CrossPlatformAction::EXIT:
            return "X";
        case CrossPlatformAction::DOWN:
            return "Down";
        case CrossPlatformAction::UP:
            return "Up";
        case CrossPlatformAction::LEFT:
            return "Left";
        case CrossPlatformAction::RIGHT:
            return "Right";
        case CrossPlatformAction::OPEN_SETTINGS:
            return "Select";
        default:
            utils::unreachable();
    } */
