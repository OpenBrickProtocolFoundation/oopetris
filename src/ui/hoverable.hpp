#pragma once

#include <core/helper/bool_wrapper.hpp>
#include <core/helper/types.hpp>
#include <core/helper/utils.hpp>

#include "graphics/rect.hpp"
#include "input/input.hpp"

namespace ui {

    enum class ActionType : u8 { Hover, Clicked, Released };

    struct Hoverable {

    private:
        bool m_is_hovered{ false };
        shapes::URect m_fill_rect;


    public:
        explicit Hoverable(const shapes::URect& fill_rect);

        Hoverable(const Hoverable&) = delete;
        Hoverable(Hoverable&&) = delete;
        Hoverable& operator=(const Hoverable&) = delete;
        Hoverable& operator=(Hoverable&&) = delete;
        virtual ~Hoverable();

        [[nodiscard]] bool is_hovered() const;

        [[nodiscard]] const shapes::URect& fill_rect() const;

        [[nodiscard]] helper::BoolWrapper<ui::ActionType>
        detect_hover(const std::shared_ptr<input::InputManager>& input_manager, const SDL_Event& event);

        void on_hover();

        void on_unhover();
    };


} // namespace ui
