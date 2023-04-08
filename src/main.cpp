#include "local_multiplayer.hpp"
#include "tetris_application.hpp"
#include <SDL.h>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <tl/optional.hpp>
#include <tuple>
#include <vector>

enum class PlayMode {
    SinglePlayer,
    LocalMultiplayer,
};


void usage(std::string prog_name) {
    std::cout << "usage:\n"
              << prog_name << " [recorded_file_path] SinglePlayer with optional recording replayer\n"
              << prog_name
              << " --multi_player <server|client> Multiplayer with 2 players (atm) with the required argument, if you "
                 "are a server or client\n";
}

std::tuple<tl::optional<std::filesystem::path>, PlayMode, bool> parse_args(std::vector<std::string> args) {


    switch (args.size()) {
        case 1: {
            return { tl::nullopt, PlayMode::SinglePlayer, false };
        }
        case 2: {
            return { args.at(1), PlayMode::SinglePlayer, false };
        }
        case 3: {
            if ("--multi_player" != args.at(1)) {
                usage(args.at(0));
                std::exit(3);
            }

            if (args.at(2) != "client" and args.at(2) != "server") {
                usage(args.at(0));
                std::exit(3);
            }

            return { tl::nullopt, PlayMode::LocalMultiplayer, args.at(2) == "server" };
        }
        default: {
            usage(args.at(0));
            std::exit(2);
        }
    }
}

int main(int argc, char** argv) {
    static constexpr int target_fps = 60;

    auto args = std::vector<std::string>{};
    for (auto i = 0; i < argc; ++i) {
        args.emplace_back(argv[i]);
    }

    const auto [rec_path, mode, is_server] = parse_args(args);
    switch (mode) {
        case PlayMode::SinglePlayer: {
            auto play_manager = std::make_unique<SinglePlayer>();
            TetrisApplication tetris_app = TetrisApplication{ std::move(play_manager), rec_path };
            tetris_app.run(target_fps);
            return 0;
        }

        case PlayMode::LocalMultiplayer: {
            auto play_manager = std::make_unique<LocalMultiplayer>(2, is_server);

            TetrisApplication tetris_app = TetrisApplication{ std::move(play_manager), tl::nullopt };
            tetris_app.run(target_fps);
            return 0;
        }
        default: {
            usage(args.at(0));
            return 4;
        }
    }
}
