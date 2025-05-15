#pragma once

#if defined(_MSC_VER) || defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#if defined(OOPETRIS_LIBRARY_GRAPHICS_TYPE) && OOPETRIS_LIBRARY_GRAPHICS_TYPE == 0
#if defined(OOPETRIS_LIBRARY_GRAPHICS_EXPORT)
#define OOPETRIS_GRAPHICS_EXPORTED __declspec(dllexport)
#else
#define OOPETRIS_GRAPHICS_EXPORTED __declspec(dllimport)
#endif
#else
#define OOPETRIS_GRAPHICS_EXPORTED __attribute__((visibility("default")))
#endif
#else
#define OOPETRIS_GRAPHICS_EXPORTED
#endif
