#pragma once

#if defined(_MSC_VER)
#define OOPETRIS_API
#else
#define OOPETRIS_API __attribute__((visibility("default")))
#endif
