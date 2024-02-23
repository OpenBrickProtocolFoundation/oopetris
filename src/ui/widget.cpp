
#include "widget.hpp"


[[nodiscard]] helper::optional<ui::Focusable*> ui::as_focusable(ui::Widget* const widget) {
    auto* const focusable = dynamic_cast<ui::Focusable*>(widget);
    if (focusable == nullptr) {
        return helper::nullopt;
    }

    return focusable;
}

[[nodiscard]] helper::optional<ui::Hoverable*> ui::as_hoverable(ui::Widget* const widget) {
    auto* const hoverable = dynamic_cast<ui::Hoverable*>(widget);
    if (hoverable == nullptr) {
        return helper::nullopt;
    }

    return hoverable;
}
