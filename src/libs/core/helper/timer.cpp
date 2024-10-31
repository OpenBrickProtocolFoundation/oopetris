

#include "./timer.hpp"


helper::Timer::Timer(Callback callback, const Duration& interval)
    : m_callback{ std::move(callback) },
      m_interval{ interval } { }

void helper::Timer::start() {
    m_running = true;
    m_last_measured = Clock::now();
}

void helper::Timer::stop() {
    m_running = false;
}

void helper::Timer::check() {
    if (not m_running) {
        return;
    }

    const auto now = Clock::now();

    const auto difference = now - m_last_measured;

    if (difference >= m_interval) {
        m_callback();
        m_last_measured = now;
    }
}
