
#include "widget.hpp"
#include "helper/utils.hpp"


[[nodiscard]] helper::optional<ui::Focusable*> ui::as_focusable(ui::Widget* const widget) {
    return utils::is_child_class<ui::Focusable>(widget);
}

[[nodiscard]] helper::optional<ui::Hoverable*> ui::as_hoverable(ui::Widget* const widget) {
    return utils::is_child_class<ui::Hoverable>(widget);
}
