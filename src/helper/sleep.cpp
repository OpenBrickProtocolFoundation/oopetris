


#include "sleep.hpp"

#if defined(_MSC_VER)
#include <windows.h>
#else
#include <cerrno>
#include <ctime>

using namespace std::chrono_literals;

#endif


bool helper::sleep_nanoseconds(std::chrono::nanoseconds ns) {

#if defined(_MSC_VER)
    // windows supports more than 100 ns precision, see here: https://randomascii.wordpress.com/2020/10/04/windows-timer-resolution-the-great-rule-change/

    HANDLE timer{};
    LARGE_INTEGER li{};

    timer = CreateWaitableTimer(nullptr, true, nullptr);
    if (timer == nullptr) {
        return false;
    }
    // use negative ns to set relative intervals
    li.QuadPart = -ns.count();
    if (!SetWaitableTimer(timer, &li, 0, nullptr, nullptr, false)) {
        CloseHandle(timer);
        return false;
    }

    WaitForSingleObject(timer, INFINITE);

    CloseHandle(timer);

    return true;

#else
    int result{};
    struct timespec remaining { };
    struct timespec current {
        static_cast<decltype(remaining.tv_sec)>(std::chrono::duration_cast<std::chrono::seconds>(ns).count()),
                static_cast<decltype(remaining.tv_nsec)>(
                        ns.count() % std::chrono::duration_cast<std::chrono::nanoseconds>(1s).count()
                ),
    };

    do { // NOLINT(cppcoreguidelines-avoid-do-while)
        result = nanosleep(&current, &remaining);

        if (result == 0) {
            return true;
        }

        if (errno != EINTR) {
            return false;
        }

        current = remaining;
    } while (true);


#endif
}
