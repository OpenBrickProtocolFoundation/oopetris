#include "tetris_application.hpp"

int main(int argc, char** argv) {
    static constexpr int target_fps = 60;

    const auto recording_path = [&]() -> tl::optional<std::filesystem::path> {
        if (argc == 2) {
            return argv[1];
        } else {
            return {};
        }
    }();

    TetrisApplication tetris_app(recording_path);
    tetris_app.run(target_fps);

    return 0;
}
