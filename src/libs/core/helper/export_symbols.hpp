#pragma once

// see https://gcc.gnu.org/wiki/Visibility
#include <core/helper/export_helper.hpp>

#if defined(OOPETRIS_LIBRARY_CORE_TYPE) && OOPETRIS_LIBRARY_CORE_TYPE == 0
#if defined(OOPETRIS_LIBRARY_CORE_EXPORT)
#define OOPETRIS_CORE_EXPORTED OOPETRIS_DLL_EXPORT
#else
#define OOPETRIS_CORE_EXPORTED OOPETRIS_DLL_IMPORT
#endif
#else
#define OOPETRIS_CORE_EXPORTED OOPETRIS_ONLY_LOCAL
#endif
