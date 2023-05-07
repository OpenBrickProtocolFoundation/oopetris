#pragma once
#include "../types.hpp"
#include <cassert>

namespace ui {

    struct Focusable {
    private:
        usize m_focus_id;
        bool m_has_focus{ false };

    public:
        explicit Focusable(usize focus_id) : m_focus_id{ focus_id } { }
        Focusable(const Focusable&) = delete;
        Focusable(Focusable&&) = delete;
        Focusable& operator=(const Focusable&) = delete;
        Focusable& operator=(Focusable&&) = delete;
        virtual ~Focusable() = default;

        [[nodiscard]] auto has_focus() const {
            return m_has_focus;
        }

        [[nodiscard]] auto focus_id() const {
            return m_focus_id;
        }

        void focus() {
            assert(not has_focus());
            m_has_focus = true;
            on_focus();
        }

        void unfocus() {
            assert(has_focus());
            m_has_focus = false;
            on_unfocus();
        }

    private:
        virtual void on_focus() = 0;
        virtual void on_unfocus() = 0;
    };

} // namespace ui
