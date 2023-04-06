#include "local_multiplayer.hpp"
#include "tetris_application.hpp"
#include <SDL.h>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>

int main(int argc, char** argv) {
    static constexpr int target_fps = 60;

    // TODO show buttons to select from one of the RunTypes

    if (argc != 2) {
        std::cerr << "no argument provided, playing single-player\n";
        auto play_manager = std::make_unique<SinglePlayer>();
        TetrisApplication tetris_app = TetrisApplication{ std::move(play_manager) };

        tetris_app.run(target_fps);

        return 0;
    }

    std::string argument = std ::string{ argv[1] };

    if (argument != "client" && argument != "server") {
        std::cerr << "you have to be either server or client, but you provided: " << argument << "\n";
        std::exit(2);
    }

    bool is_server = argument == "server";

    auto play_manager = std::make_unique<LocalMultiplayer>(2, is_server);

    TetrisApplication tetris_app = TetrisApplication{ std::move(play_manager) };

    tetris_app.run(target_fps);

    return 0;
}
