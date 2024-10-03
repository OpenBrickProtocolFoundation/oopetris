#pragma once

#include "./windows.hpp"

#include <chrono>

namespace helper {

    OOPETRIS_CORE_EXPORTED bool sleep_nanoseconds(std::chrono::nanoseconds nano_seconds);
}
