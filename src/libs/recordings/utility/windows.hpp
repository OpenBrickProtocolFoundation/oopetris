#pragma once

#if  defined(_MSC_VER) || defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#if defined(OOPETRIS_LIBRARY_RECORDINGS_TYPE) && OOPETRIS_LIBRARY_RECORDINGS_TYPE == 0
#if defined(OOPETRIS_LIBRARY_RECORDINGS_EXPORT)
#if defined(__GNUC__)
#define OOPETRIS_RECORDINGS_EXPORTED __attribute__((dllexport))
#else
#define OOPETRIS_RECORDINGS_EXPORTED __declspec(dllexport)
#endif
#else
#if defined(__GNUC__)
#define OOPETRIS_RECORDINGS_EXPORTED __attribute__((dllimport))
#else
#define OOPETRIS_RECORDINGS_EXPORTED __declspec(dllimport)
#endif
#endif
#else
#define OOPETRIS_RECORDINGS_EXPORTED  __attribute__((visibility("default")))
#endif
#else
#define OOPETRIS_RECORDINGS_EXPORTED
#endif
