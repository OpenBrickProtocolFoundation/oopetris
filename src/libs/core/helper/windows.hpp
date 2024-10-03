#pragma once

#if defined(_MSC_VER)
#if defined(OOPETRIS_LIBRARY_CORE_EXPORT)
#define OOPETRIS_CORE_EXPORTED __declspec(dllexport)
#else
#define OOPETRIS_CORE_EXPORTED __declspec(dllimport)
#endif
#else
#define OOPETRIS_CORE_EXPORTED
#endif
