#pragma once

#include "helper/command_line_arguments.hpp"
#include "input/event_listener.hpp"
#include "manager/service_provider.hpp"
#include "manager/settings.hpp"
#include "scene_id.hpp"
#include "ui/layout.hpp"

#include <functional>


namespace scenes {
    enum class SceneUpdate {
        ContinueUpdating,
        StopUpdating,
    };

    struct Scene {
    public:
        struct Switch {
            SceneId target_scene;
            ui::Layout layout;

            Switch(const SceneId target_scene, const ui::Layout& layout)
                : target_scene{ target_scene },
                  layout{ layout } { }
        };

        struct Push {
            SceneId target_scene;
            ui::Layout layout;

            Push(const SceneId target_scene, const ui::Layout& layout)
                : target_scene{ target_scene },
                  layout{ layout } { }
        };

        struct Pop { };

        struct Exit { };

        using Change = std::variant<Pop, Push, Switch, Exit>;
        using UpdateResult = std::pair<SceneUpdate, helpers::optional<Change>>;

    protected:
        ServiceProvider* m_service_provider;

    private:
        ui::Layout m_layout;

    public:
        explicit Scene(ServiceProvider* service_provider, const ui::Layout& layout);
        Scene(const Scene&) = delete;
        Scene& operator=(const Scene&) = delete;
        virtual ~Scene() = default;

        [[nodiscard]] virtual UpdateResult update() = 0;
        virtual void render(const ServiceProvider& service_provider) = 0;
        virtual bool handle_event(const SDL_Event& event, const Window* window) = 0;
        [[nodiscard]] const ui::Layout& get_layout() const;
    };

    [[nodiscard]] std::unique_ptr<scenes::Scene>
    create_scene(ServiceProvider& service_provider, SceneId identifier, const ui::Layout& layout);
} // namespace scenes
