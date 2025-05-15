
#pragma once

// see: https://gcc.gnu.org/wiki/Visibility

// clang-format off
#if defined(_MSC_VER) || defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    #define OOPETRIS_ONLY_LOCAL

    #if defined(__GNUC__)
        #define OOPETRIS_DLL_EXPORT __attribute__((dllexport))
        #define OOPETRIS_DLL_IMPORT __attribute__((dllimport))
    #else
        #define OOPETRIS_DLL_EXPORT __declspec(dllexport) 
        #define OOPETRIS_DLL_IMPORT __declspec(dllimport) 
    #endif
#else
    #define OOPETRIS_ONLY_LOCAL __attribute__((visibility("hidden")))

    #define OOPETRIS_DLL_EXPORT __attribute__((visibility("default")))
    #define OOPETRIS_DLL_IMPORT __attribute__((visibility("default")))
#endif
// clang-format on
