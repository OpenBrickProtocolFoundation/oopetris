#pragma once

#if defined(_MSC_VER)
#if defined(OOPETRIS_LIBRARY_RECORDINGS_TYPE) && OOPETRIS_LIBRARY_RECORDINGS_TYPE == 0
#if defined(OOPETRIS_LIBRARY_RECORDINGS_EXPORT)
#define OOPETRIS_RECORDINGS_EXPORTED __declspec(dllexport)
#else
#define OOPETRIS_RECORDINGS_EXPORTED __declspec(dllimport)
#endif
#else
#define OOPETRIS_RECORDINGS_EXPORTED
#endif
#else
#define OOPETRIS_RECORDINGS_EXPORTED
#endif
