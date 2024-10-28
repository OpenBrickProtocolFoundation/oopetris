#include "settings.hpp"
#include "helper/graphic_utils.hpp"
#include "input/keyboard_input.hpp"
#include "input/touch_input.hpp"


#include <spdlog/spdlog.h>


void settings::to_json(nlohmann::json& obj, const settings::Settings& settings) {
    obj = nlohmann::json{
        { "controls",
         nlohmann::json{ { "inputs", settings.controls }, { "selected", settings.selected } },
         { "volume", settings.volume },
         { "discord", settings.discord },
         { "api_url", settings.api_url } }
    };
}

void settings::from_json(const nlohmann::json& obj, settings::Settings& settings) {

    ::json::check_for_no_additional_keys(obj, { "controls", "volume", "discord", "api_url" });

    obj.at("volume").get_to(settings.volume);
    obj.at("api_url").get_to(settings.api_url);
    obj.at("discord").get_to(settings.discord);

    const auto& controls = obj.at("controls");

    ::json::check_for_no_additional_keys(controls, { "inputs", "selected" });

    controls.at("inputs").get_to(settings.controls);
    controls.at("selected").get_to(settings.selected);
}
