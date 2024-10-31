

#pragma once

#include <chrono>
#include <functional>

#include "./windows.hpp"

namespace helper {

    struct Timer {
    private:
        using Callback = std::function<void()>;
        using Clock = std::chrono::high_resolution_clock;
        using Duration = std::chrono::nanoseconds;

        bool m_running{ false };
        std::chrono::time_point<Clock> m_last_measured;
        Callback m_callback;
        Duration m_interval;


    public:
        OOPETRIS_CORE_EXPORTED Timer(Callback callback, const Duration& interval);

        OOPETRIS_CORE_EXPORTED void start();

        OOPETRIS_CORE_EXPORTED void stop();

        OOPETRIS_CORE_EXPORTED void check();
    };


} // namespace helper
