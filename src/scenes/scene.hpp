#pragma once

#include "game/command_line_arguments.hpp"
#include "input/input.hpp"
#include "manager/event_listener.hpp"
#include "manager/service_provider.hpp"
#include "scene_id.hpp"
#include "ui/layout.hpp"

#include <functional>
#include <variant>

namespace scenes {
    enum class SceneUpdate : u8 {
        ContinueUpdating,
        StopUpdating,
    };

    struct Scene {
    public:
        struct Switch {
            SceneId m_target_scene;
            ui::Layout m_layout;

            Switch(const SceneId target_scene, const ui::Layout& layout)
                : m_target_scene{ target_scene },
                  m_layout{ layout } { }
        };

        struct RawSwitch {
            std::string name;
            std::unique_ptr<Scene> scene;

            RawSwitch(std::string name, std::unique_ptr<Scene>&& scene)
                : name{ std::move(name) },
                  scene{ std::move(scene) } { }
        };

        struct Push {
            SceneId target_scene;
            ui::Layout layout;

            Push(const SceneId target_scene, const ui::Layout& layout)
                : target_scene{ target_scene },
                  layout{ layout } { }
        };

        struct RawPush {
            std::string name;
            std::unique_ptr<Scene> scene;

            RawPush(std::string name, std::unique_ptr<Scene>&& scene)
                : name{ std::move(name) },
                  scene{ std::move(scene) } { }
        };

        struct Pop { };

        struct Exit { };

        using Change = std::variant<Pop, Push, RawPush, Switch, RawSwitch, Exit>;
        using UpdateResult = std::pair<SceneUpdate, std::optional<Change>>;

    protected:
        ServiceProvider*
                m_service_provider; //NOLINT(misc-non-private-member-variables-in-classes,cppcoreguidelines-non-private-member-variables-in-classes)

    private:
        ui::Layout m_layout;

    public:
        OOPETRIS_GRAPHICS_EXPORTED explicit Scene(ServiceProvider* service_provider, const ui::Layout& layout);
        Scene(const Scene&) = delete;
        Scene& operator=(const Scene&) = delete;
        virtual ~Scene() = default;

        [[nodiscard]] virtual UpdateResult update() = 0;
        virtual void render(const ServiceProvider& service_provider) = 0;
        virtual bool
        handle_event(const std::shared_ptr<input::InputManager>& input_manager, const SDL_Event& event) = 0;
        // override this, if you (the scene) could potentially be displayed in non fullscreen!
        OOPETRIS_GRAPHICS_EXPORTED virtual void on_unhover();
        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] const ui::Layout& get_layout() const;
    };

    OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] std::unique_ptr<scenes::Scene>
    create_scene(ServiceProvider& service_provider, SceneId identifier, const ui::Layout& layout);
} // namespace scenes
