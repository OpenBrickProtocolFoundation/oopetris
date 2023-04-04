#pragma once

#include <tl/optional.hpp>

struct Input {
    enum class Event {
        RotateLeft,
        RotateRight,
        MoveLeft,
        MoveRight,
        MoveDown,
        Drop,
    };

protected:
    using Callback = Event(*)();

    Callback m_callback;

public:
    virtual ~Input() = default;
};
