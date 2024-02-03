#pragma once
#include "../types.hpp"
#include <cassert>

namespace ui {

    struct Hoverable {
    private:
        usize m_focus_id;
        bool m_has_focus{ false };

    public:
        explicit Hoverable(usize focus_id) : m_focus_id{ focus_id } { }
        Hoverable(const Hoverable&) = delete;
        Hoverable(Hoverable&&) = delete;
        Hoverable& operator=(const Hoverable&) = delete;
        Hoverable& operator=(Hoverable&&) = delete;
        virtual ~Hoverable() = default;

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
