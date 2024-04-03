

#pragma once

#include "scenes/scene.hpp"

namespace settings {

    struct SettingsDetails {
        [[nodiscard]] virtual scenes::Scene::Change get_details_scene() = 0;
    };


} // namespace settings
