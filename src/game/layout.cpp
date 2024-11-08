
#include "./layout.hpp"

std::vector<ui::Layout> game::get_layouts_for(u32 players, const ui::Layout& layout) {

    std::vector<ui::Layout> layouts{};
    layouts.reserve(players);

    if (players == 0) {
        throw std::runtime_error("An empty recording file isn't supported");
    } else if (players == 1) { // NOLINT(readability-else-after-return,llvm-else-after-return)
        layouts.push_back(ui::RelativeLayout{ layout, 0.02, 0.01, 0.96, 0.98 });
    } else if (players == 2) {
        layouts.push_back(ui::RelativeLayout{ layout, 0.02, 0.01, 0.46, 0.98 });
        layouts.push_back(ui::RelativeLayout{ layout, 0.52, 0.01, 0.46, 0.98 });
    } else {

        //TODO(Totto): support bigger layouts than just 2
        throw std::runtime_error("At the moment only replays from up to two players are supported");
    }

    return layouts;
}
