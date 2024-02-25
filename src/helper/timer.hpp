

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
        std::chrono::time_point<Clock> last_measured;
        Callback m_callback;
        Duration m_interval;


    public:
        Timer(Callback callback, const Duration& interval)
            : m_callback{ std::move(callback) },
              m_interval{ interval } { }

        void start() {
            m_running = true;
            last_measured = Clock::now();
        }

        void stop() {
            m_running = false;
        }

        void check() {
            if (not m_running) {
                return;
            }

            const auto now = Clock::now();

            const auto difference = now - last_measured;

            if (difference >= m_interval) {
                m_callback();
                last_measured = now;
            }
        }
    };


} // namespace helper
