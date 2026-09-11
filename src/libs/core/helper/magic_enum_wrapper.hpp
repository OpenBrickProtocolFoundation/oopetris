#pragma once

#ifndef MAGIC_ENUM_RANGE_MIN
#define MAGIC_ENUM_RANGE_MIN 0 // NOLINT(cppcoreguidelines-macro-usage)
#endif

#ifndef MAGIC_ENUM_RANGE_MAX
#define MAGIC_ENUM_RANGE_MAX 256 // NOLINT(cppcoreguidelines-macro-usage)
#endif

#ifdef NDEBUG
#define MAGIC_ENUM_NO_ASSERT
#endif

#include <magic_enum/magic_enum.hpp>
