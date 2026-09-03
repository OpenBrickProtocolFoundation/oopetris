

#pragma once


#if !defined(__UEFI__)
#error "this header is for uefi only"
#endif


#include "manager/service_provider.hpp"

#include "helper/spdlog_wrapper.hpp"
#include <optional>
#include <spdlog/sinks/ansicolor_sink.h>
#include <spdlog/sinks/callback_sink.h>
#include <string>


#include <core/helper/uefi_debug.h>


namespace uefi {

    namespace helper {

        //TODO: we could use a c++ wrapper of the SPIN_LOCK from "SynchronizationLib.h" instead, or is that not atomic?
        struct ThreadMutex {
        private:
            volatile UINT32 locked;

        public:
            ThreadMutex() noexcept;

            ThreadMutex(const ThreadMutex& other) = delete;
            ThreadMutex& operator=(const ThreadMutex& other) = delete;

            ThreadMutex(ThreadMutex&& other) noexcept;
            ThreadMutex& operator=(ThreadMutex&& other) noexcept;

            ~ThreadMutex() noexcept;

            void lock();
            void unlock();
        };

    } // namespace helper

    namespace spdlog_helper {
        using callback_sink = spdlog::sinks::callback_sink<uefi::helper::ThreadMutex>;

        namespace details {
            struct console_mutex {
                using mutex_t = uefi::helper::ThreadMutex;
                static mutex_t& mutex() {
                    static mutex_t s_mutex;
                    return s_mutex;
                }
            };
        } // namespace details

        using stdout_color_sink = spdlog::sinks::ansicolor_stdout_sink<details::console_mutex>;
        using stderr_color_sink = spdlog::sinks::ansicolor_stderr_sink<details::console_mutex>;
    } // namespace spdlog_helper


    [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED std::shared_ptr<uefi::spdlog_helper::callback_sink> get_debug_sink();

    [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED std::string map_efi_status_to_string(EFI_STATUS status);

    OOPETRIS_GRAPHICS_EXPORTED void platform_init();

    OOPETRIS_GRAPHICS_EXPORTED void platform_exit();

}; // namespace uefi
