#include "ai_game.hpp"
#include "external/load_file.hpp"
#include "graphics/window.hpp"
#include "helper/constants.hpp"
#include "helper/git_helper.hpp"
#include "helper/utils.hpp"
#include "manager/resource_manager.hpp"

#include <fmt/format.h>

namespace scenes {

    AIGame::AIGame(ServiceProvider* service_provider, const ui::Layout& layout) : Scene{ service_provider, layout } {

        external::load_file(std::filesystem::path("/home/totto/Code/coder2k/oopetris_pr5/src/external/examples/ai.lua"));
    }

    [[nodiscard]] Scene::UpdateResult AIGame::update() {
        if (m_should_exit) {
            return UpdateResult{ SceneUpdate::StopUpdating, Scene::Pop{} };
        }
        return UpdateResult{ SceneUpdate::StopUpdating, helper::nullopt };
    }

    void AIGame::render(const ServiceProvider&) { }

    bool AIGame::handle_event(const SDL_Event& event, const Window*) {
        if (utils::event_is_action(event, utils::CrossPlatformAction::CLOSE)) {
            m_should_exit = true;
            return true;
        }

        return false;
    }

} // namespace scenes
