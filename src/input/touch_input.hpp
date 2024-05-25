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
        SDL_TouchID m_id;

    public:
        TouchInput(const std::shared_ptr<Window>& window, SDL_TouchID touch_id, const std::string& name);

        [[nodiscard]] static helper::expected<std::unique_ptr<TouchInput>, std::string>
        get_by_device_index(const std::shared_ptr<Window>& window, int device_index);

        [[nodiscard]] std::optional<NavigationEvent> get_navigation_event(const SDL_Event& event) const override;

        [[nodiscard]] std::string describe_navigation_event(NavigationEvent event) const override;

        [[nodiscard]] std::optional<PointerEventHelper> get_pointer_event(const SDL_Event& event) const override;

        [[nodiscard]] SDL_Event offset_pointer_event(const SDL_Event& event, const shapes::IPoint& point)
                const override;
    };


    struct TouchInputManager {
        static void
        discover_devices(std::vector<std::unique_ptr<Input>>& inputs, const std::shared_ptr<Window>& window);
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
        explicit PressedState(Uint32 timestamp, float x_pos, float y_pos) //NOLINT(bugprone-easily-swappable-parameters)
            : timestamp{ timestamp },
              x{ x_pos },
              y{ y_pos } { }
    };

    struct TouchGameInput final : public GameInput, public EventListener {

    private:
        TouchSettings m_settings;

        std::unordered_map<SDL_FingerID, std::optional<PressedState>> m_finger_state;
        std::vector<SDL_Event> m_event_buffer;
        EventDispatcher* m_event_dispatcher;
        TouchInput* m_underlying_input;

    public:
        explicit TouchGameInput(
                const TouchSettings& settings,
                EventDispatcher* event_dispatcher,
                TouchInput* underlying_input
        );

        ~TouchGameInput() override;


        TouchGameInput(const TouchGameInput& input) = delete;
        [[nodiscard]] TouchGameInput& operator=(const TouchGameInput& input) = delete;

        TouchGameInput(TouchGameInput&& input) noexcept;
        [[nodiscard]] TouchGameInput& operator=(TouchGameInput&& input) noexcept;

        void handle_event(const SDL_Event& event) override;
        void update(SimulationStep simulation_step_index) override;

        [[nodiscard]] std::optional<MenuEvent> get_menu_event(const SDL_Event& event) const override;

        [[nodiscard]] std::string describe_menu_event(MenuEvent event) const override;

        [[nodiscard]] const TouchInput* underlying_input() const override;

    private:
        [[nodiscard]] std::optional<InputEvent> sdl_event_to_input_event(const SDL_Event& event);
    };

} // namespace input


namespace json_helper {

    template<typename T>
    concept IsNumeric = std::is_arithmetic_v<T>;


    template<IsNumeric T>
    [[nodiscard]] T get_number(const nlohmann::json& obj, const std::string& name) {

        helper::expected<bool, std::string> error = true;

        auto context = obj.at(name);


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
        static input::TouchSettings from_json(const json& obj) {


            ::json::check_for_no_additional_keys(
                    obj,
                    {
                            "type",
                            "move_x_threshold",
                            "move_y_threshold",
                            "rotation_duration_threshold",
                            "drop_duration_threshold",
                    }
            );

            const auto move_x_threshold = json_helper::get_number<double>(obj, "move_x_threshold");
            const auto move_y_threshold = json_helper::get_number<double>(obj, "move_y_threshold");

            const auto rotation_duration_threshold = json_helper::get_number<u32>(obj, "rotation_duration_threshold");
            const auto drop_duration_threshold = json_helper::get_number<u32>(obj, "drop_duration_threshold");

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

        static void to_json(json& obj, const input::TouchSettings& settings) {

            obj = nlohmann::json{
                {            "move_x_threshold",            settings.move_x_threshold },
                {            "move_y_threshold",            settings.move_y_threshold },
                { "rotation_duration_threshold", settings.rotation_duration_threshold },
                {     "drop_duration_threshold",     settings.drop_duration_threshold },
            };
        }
    };
} // namespace nlohmann
