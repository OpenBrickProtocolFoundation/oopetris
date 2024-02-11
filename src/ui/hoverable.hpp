#pragma once
#include "helper/types.hpp"
#include <cassert>

namespace ui {

    struct Hoverable {
    private:
        bool m_is_hovered{ false };

    public:
        explicit Hoverable() = default;
        Hoverable(const Hoverable&) = delete;
        Hoverable(Hoverable&&) = delete;
        Hoverable& operator=(const Hoverable&) = delete;
        Hoverable& operator=(Hoverable&&) = delete;
        virtual ~Hoverable() = default;

        [[nodiscard]] auto is_hovered() const {
            return m_is_hovered;
        }


    protected:
        void on_hover() {
            m_is_hovered = true;
        }

        void on_unhover() {
            m_is_hovered = false;
        }
    };

} // namespace ui
