#pragma once


#include "helper/expected.hpp"
#include "helper/parse_json.hpp"
#include "input.hpp"
#include "input/game_input.hpp"
#include "manager/event_dispatcher.hpp"
#include <limits>

namespace input {

    struct TouchInput : PointerInput {
        std::shared_ptr<Window> m_window;

    public:
        TouchInput(const std::shared_ptr<Window>& window);
        virtual ~TouchInput();


        [[nodiscard]] helper::optional<NavigationEvent> get_navigation_event(const SDL_Event& event) const override;

        [[nodiscard]] std::string describe_navigation_event(NavigationEvent event) const override;

        [[nodiscard]] helper::optional<PointerEventHelper> get_pointer_event(const SDL_Event& event) const override;

        [[nodiscard]] SDL_Event offset_pointer_event(const SDL_Event& event, const shapes::IPoint& point) const override;
    };


    struct TouchSettings {
        double move_x_threshold;
        double move_y_threshold;

        // in ms
        u32 rotation_duration_threshold;
        u32 drop_duration_threshold;

        [[nodiscard]] helper::expected<bool, std::string> validate() const;

        [[nodiscard]] static TouchSettings default_settings() {
            return TouchSettings{ .move_x_threshold = 150.0 / 2160.0,
                                  .move_y_threshold = 400.0 / 1080.0,
                                  .rotation_duration_threshold = 500,
                                  .drop_duration_threshold = 200 };
        }
    };

    struct PressedState {
        Uint32 timestamp;
        float x;
        float y;
        explicit PressedState(Uint32 timestamp, float x, float y) : timestamp{ timestamp }, x{ x }, y{ y } { }
    };

    struct TouchGameInput final : public GameInput, public EventListener {

    private:
        TouchSettings m_settings;

        std::unordered_map<SDL_FingerID, helper::optional<PressedState>> m_finger_state;
        std::vector<SDL_Event> m_event_buffer;
        EventDispatcher* m_event_dispatcher;

    public:
        explicit TouchGameInput(const TouchSettings& settings, EventDispatcher* event_dispatcher)
            : GameInput{ GameInputType::Touch },
              m_settings{ settings },
              m_event_dispatcher{ event_dispatcher } {
            m_event_dispatcher->register_listener(this);
        }

        ~TouchGameInput() override {
            m_event_dispatcher->unregister_listener(this);
        }

        void handle_event(const SDL_Event& event) override;
        void update(SimulationStep simulation_step_index) override;

        [[nodiscard]] helper::optional<MenuEvent> get_menu_event(const SDL_Event& event) const override;

        [[nodiscard]] std::string describe_menu_event(MenuEvent event) const override;

    private:
        [[nodiscard]] helper::optional<InputEvent> sdl_event_to_input_event(const SDL_Event& event);
    };

} // namespace input


namespace json_helper {

    template<typename T>
    concept IsNumeric = std::is_arithmetic_v<T>;


    template<IsNumeric T>
    [[nodiscard]] T get_number(const nlohmann::json& j, const std::string& name) {

        helper::expected<bool, std::string> error = true;

        auto context = j.at(name);


        if (not context.is_number()) {
            error = helper::unexpected<std::string>{ fmt::format("Not a number but: {}", context.type_name()) };
        }

        if (error.has_value()) {


            // integers are checked explicitly, floating points are just retrieved vai get_to, which may convert numbers, but converting from int to floating point is always fine

            if constexpr (std::numeric_limits<T>::is_integer) {

                if (not context.is_number_integer()) {
                    error = helper::unexpected<std::string>{ "Not an integer" };
                } else {
                    if constexpr (not std::numeric_limits<T>::is_signed) {
                        if (not context.is_number_unsigned()) {
                            error = helper::unexpected<std::string>{ "Not an unsigned integer" };
                        }
                    }
                }
            }
        }

        if (not error.has_value()) {

            throw nlohmann::json::type_error::create(
                    302,
                    fmt::format(
                            "Expected a valid number of type '{}' for key '{}' but an error occurred: {}",
                            typeid(T).name(), name, error.error()
                    ),
                    &context
            );
        }

        T input;
        context.get_to(input);


        return input;
    }

} // namespace json_helper


namespace nlohmann {
    template<>
    struct adl_serializer<input::TouchSettings> {
        static input::TouchSettings from_json(const json& j) {


            ::json::check_for_no_additional_keys(
                    j,
                    {
                            "type",
                            "move_x_threshold",
                            "move_y_threshold",
                            "rotation_duration_threshold",
                            "drop_duration_threshold",
                    }
            );

            const auto move_x_threshold = json_helper::get_number<double>(j, "move_x_threshold");
            const auto move_y_threshold = json_helper::get_number<double>(j, "move_y_threshold");

            const auto rotation_duration_threshold = json_helper::get_number<u32>(j, "rotation_duration_threshold");
            const auto drop_duration_threshold = json_helper::get_number<u32>(j, "drop_duration_threshold");

            auto settings = input::TouchSettings{ .move_x_threshold = move_x_threshold,
                                                  .move_y_threshold = move_y_threshold,
                                                  .rotation_duration_threshold = rotation_duration_threshold,
                                                  .drop_duration_threshold = drop_duration_threshold };


            const auto is_valid = settings.validate();
            if (not is_valid.has_value()) {
                throw std::runtime_error(is_valid.error());
            }

            return settings;
        }

        static void to_json(json& j, const input::TouchSettings& settings) {

            j = nlohmann::json{
                {            "move_x_threshold",            settings.move_x_threshold },
                {            "move_y_threshold",            settings.move_y_threshold },
                { "rotation_duration_threshold", settings.rotation_duration_threshold },
                {     "drop_duration_threshold",     settings.drop_duration_threshold },
            };
        }
    };
} // namespace nlohmann


//TODO:
/* 


    decltype(event.type) desired_type{};
    switch (click_type) {
        case CrossPlatformClickEvent::Motion:
            desired_type = SDL_FINGERMOTION;
            break;
        case CrossPlatformClickEvent::ButtonDown:
            desired_type = SDL_FINGERDOWN;
            break;
        case CrossPlatformClickEvent::ButtonUp:
            desired_type = SDL_FINGERUP;
            break;
        case CrossPlatformClickEvent::Any:
            return event.type == SDL_FINGERMOTION || event.type == SDL_FINGERDOWN || event.type == SDL_FINGERUP;
        default:
            utils::unreachable();
    }

    return event.type == desired_type;
 */


/**
 


[[nodiscard]] std::pair<i32, i32> utils::get_raw_coordinates(const Window* window, const SDL_Event& event) {

    assert(utils::event_is_click_event(event, utils::CrossPlatformClickEvent::Any) && "expected a click event");

#if defined(__ANDROID__)
    // These are doubles, from 0-1 (or if using virtual layouts > 0) in percent, the have to be casted to absolut x coordinates!
    const double x_percent = event.tfinger.x;
    const double y_percent = event.tfinger.y;
    const auto window_size = window->size();
    const auto x = static_cast<i32>(std::round(x_percent * window_size.x));
    const auto y = static_cast<i32>(std::round(y_percent * window_size.y));


#elif defined(__SWITCH__)
    UNUSED(window);
    UNUSED(event);
    throw std::runtime_error("Not supported on the Nintendo switch");
    int x{};
    int y{};
#else
    UNUSED(window);

    Sint32 x{};
    Sint32 y{};
    switch (event.type) {
        case SDL_MOUSEMOTION:
            x = event.motion.x;
            y = event.motion.y;
            break;
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
            x = event.button.x;
            y = event.button.y;
            break;
        default:
            utils::unreachable();
    }
#endif


    return { static_cast<i32>(x), static_cast<i32>(y) };
}


 * 
 */
