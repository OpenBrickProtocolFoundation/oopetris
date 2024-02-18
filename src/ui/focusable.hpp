#pragma once
#include "helper/types.hpp"
#include <cassert>

namespace ui {

    struct IDHelper {
    private:
        u32 m_index;
        u32 m_focus_id;

    public:
        IDHelper(u32 start_index = 0, u32 start_focus_id = 0) // NOLINT(bugprone-easily-swappable-parameters)
            : m_index{ start_index },
              m_focus_id{ start_focus_id } {};

        u32 index() {
            return m_index++;
        }

        u32 focus_id() {
            return m_focus_id++;
        }
    };

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
        virtual void on_focus(){
            //do nothing
        };
        virtual void on_unfocus() {
            //do nothing
        }
    };

} // namespace ui
