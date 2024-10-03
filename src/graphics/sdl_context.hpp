#pragma once

#include "helper/windows.hpp"

struct SdlContext final {
    OOPETRIS_GRAPHICS_EXPORTED SdlContext();
    OOPETRIS_GRAPHICS_EXPORTED SdlContext(const SdlContext&) = delete;
    OOPETRIS_GRAPHICS_EXPORTED SdlContext(SdlContext&&) = delete;
    OOPETRIS_GRAPHICS_EXPORTED SdlContext& operator=(const SdlContext&) = delete;
    OOPETRIS_GRAPHICS_EXPORTED SdlContext& operator=(SdlContext&&) = delete;
    OOPETRIS_GRAPHICS_EXPORTED ~SdlContext();
};
