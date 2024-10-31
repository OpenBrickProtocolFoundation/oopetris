

#pragma once

#include <chrono>
#include <functional>

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
        Timer(Callback callback, const Duration& interval);

        void start();

        void stop();

        void check();
    };


} // namespace helper
