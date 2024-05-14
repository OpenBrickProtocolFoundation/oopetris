
#pragma once

#ifdef _USE_FMTLIB
#include <fmt/format.h>
#else
#include <format>
#endif


#ifdef _USE_FMTLIB
#else
namespace fmt = std; 
#endif
