#pragma once

// see https://gcc.gnu.org/wiki/Visibility
#include <core/helper/export_helper.hpp>

#if defined(OOPETRIS_LIBRARY_GRAPHICS_TYPE) && OOPETRIS_LIBRARY_GRAPHICS_TYPE == 0
#if defined(OOPETRIS_LIBRARY_GRAPHICS_EXPORT)
#define OOPETRIS_GRAPHICS_EXPORTED OOPETRIS_DLL_EXPORT
#else
#define OOPETRIS_GRAPHICS_EXPORTED OOPETRIS_DLL_IMPORT
#endif
#else
#define OOPETRIS_GRAPHICS_EXPORTED OOPETRIS_ONLY_LOCAL
#endif
