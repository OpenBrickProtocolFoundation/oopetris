

#pragma once

#if defined(_MSC_VER)
#pragma gcc warning(push, 0)
#else
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wall"
#pragma GCC diagnostic ignored "-Wold-style-cast"
#endif

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>


#if defined(_MSC_VER)
#pragma gcc warning(pop)
#else
#pragma GCC diagnostic pop
#endif
