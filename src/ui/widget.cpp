
#include "widget.hpp"
#include "helper/utils.hpp"


[[nodiscard]] std::optional<ui::Focusable*> ui::as_focusable(ui::Widget* const widget) {
    return utils::is_child_class<ui::Focusable>(widget);
}

[[nodiscard]] std::optional<ui::Hoverable*> ui::as_hoverable(ui::Widget* const widget) {
    return utils::is_child_class<ui::Hoverable>(widget);
}
