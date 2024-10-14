#pragma once

#if defined(_MSC_VER)
#if defined(OOPETRIS_LIBRARY_GRAPHICS_COMPILATION)
#if defined(OOPETRIS_LIBRARY_GRAPHICS_EXPORT)
#define OOPETRIS_GRAPHICS_EXPORTED __declspec(dllexport)
#else
#define OOPETRIS_GRAPHICS_EXPORTED __declspec(dllimport)
#endif
#else
#define OOPETRIS_GRAPHICS_EXPORTED
#endif
#else
#define OOPETRIS_GRAPHICS_EXPORTED
#endif
