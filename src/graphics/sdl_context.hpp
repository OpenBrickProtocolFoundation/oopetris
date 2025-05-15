#pragma once

#include "helper/export_symbols.hpp"

struct SdlContext final {
    OOPETRIS_GRAPHICS_EXPORTED SdlContext();
    SdlContext(const SdlContext&) = delete;
    SdlContext(SdlContext&&) = delete;
    SdlContext& operator=(const SdlContext&) = delete;
    SdlContext& operator=(SdlContext&&) = delete;
    OOPETRIS_GRAPHICS_EXPORTED ~SdlContext();
};
