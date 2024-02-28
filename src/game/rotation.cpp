
#include "rotation.hpp"


Rotation& operator++(Rotation& rotation) {
    rotation = static_cast<Rotation>((static_cast<u8>(rotation) + 1) % (static_cast<u8>(Rotation::LastRotation) + 1));
    return rotation;
}

Rotation& operator--(Rotation& rotation) {
    rotation = static_cast<Rotation>(
            (static_cast<u8>(rotation) + static_cast<u8>(Rotation::LastRotation))
            % (static_cast<u8>(Rotation::LastRotation) + 1)
    );
    return rotation;
}

Rotation operator+(Rotation rotation, i8 offset) {
    if (offset == 0) {
        return rotation;
    }

    if (offset > 0) {
        for (u8 i = 0; i < offset; ++i) {
            ++rotation;
        }
        return rotation;
    }

    for (u8 i = 0; i < static_cast<u8>(-offset); ++i) {
        --rotation;
    }
    return rotation;
}

Rotation operator-(const Rotation rotation, i8 offset) {
    return rotation + (-offset);
}
