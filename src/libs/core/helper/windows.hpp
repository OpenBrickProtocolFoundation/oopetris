#pragma once

#if defined(_MSC_VER)
#if defined(OOPETRIS_LIBRARY_EXPORT)
#define OOPETRIS_EXPORTED __declspec(dllexport)
#else
#define OOPETRIS_EXPORTED __declspec(dllimport)
#endif
#else
#define OOPETRIS_EXPORTED
#endif
