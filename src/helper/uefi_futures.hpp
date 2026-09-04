

#pragma once


#if !defined(__UEFI__)
#error "this header is for uefi only"
#endif


#include <core/helper/uefi_debug.h>

extern "C" {
#include <Uefi.h>

#include <Pi/PiMultiPhase.h>

#include <Protocol/MpService.h>

#include <setjmp.h>
#include <signal.h>
}

#include "./uefi_utils.hpp"
#include <chrono>
#include <memory>
#include <mutex>
#include <spdlog/spdlog.h>
#include <variant>
#include <vector>


//COPY from oopetris, to make this header more independent
namespace helper::uefi::future {

    template<class... Ts>
    struct Overloaded : Ts... {
        using Ts::operator()...;
    };
    template<class... Ts>
    Overloaded(Ts...) -> Overloaded<Ts...>;
} // namespace helper::uefi::future


namespace details {

    struct CpuJumpState {
        jmp_buf jump_state;
    };

    struct SecondaryCPUState {
        UINT64 UniqueProcessorId;
        UINTN Id;
        UINT32 StatusFlags;
        bool IsBusy;
        std::optional<CpuJumpState> jump_state;
    };

    struct GlobalSignalState {
        __sighandler_t* old_sig_handler;
        std::mutex mutex;
    };

    struct MainCPUState {
        EFI_MP_SERVICES_PROTOCOL* Mp;
        UINTN NumberOfProcessors;
        UINTN BspId;
        std::vector<SecondaryCPUState> cpus;
        GlobalSignalState signal_state;
    };

    extern MainCPUState* __cpu_state;

    void init_cpu_state();

    [[nodiscard]] UINTN my_cpu_id();


    [[nodiscard]] SecondaryCPUState* find_cpu_for_new_thread();

    struct RunningFunctionInfo {
        //TODO: use generic fn pointer;
        std::function<void(void)> fn;
        //TODO: support arguments
        // void args;
    };

    struct ThreadInfo {
        RunningFunctionInfo info;
    };

    enum class thread_state { aborted, running, finished };

    typedef struct DetachedThreadStateImpl DetachedThreadState;


    struct DetachedThreadStatePublic {
    private:
        DetachedThreadState* m_state_impl;

    public:
        DetachedThreadStatePublic(DetachedThreadState* state_impl);

        ~DetachedThreadStatePublic();

        [[nodiscard]] thread_state poll();

        [[nodiscard]] std::string state() const;
    };

    std::shared_ptr<DetachedThreadStatePublic>
    start_detached_thread(SecondaryCPUState* const cpu_to_execute_on, const std::shared_ptr<ThreadInfo>& info);


} // namespace details


namespace uefi::helper {

    enum class future_status { ready, timeout, error };

    template<typename T>
    //TODO: only allow void!
    class future {
    private:
        struct ErrorState {
            std::string error;
        };

        struct FinishedState {
            std::conditional_t<std::is_void_v<T>, std::monostate, T> value;
        };

        struct RunningState {
            details::SecondaryCPUState* cpu;
            std::shared_ptr<details::ThreadInfo> info;
            std::shared_ptr<details::DetachedThreadStatePublic> thread;
        };

        mutable std::variant<RunningState, ErrorState, FinishedState> m_state;

        void __internal_poll(void) const {
            std::optional<std::variant<RunningState, ErrorState, FinishedState>> new_value = std::visit(
                    ::helper::uefi::future::Overloaded{
                            [this](const RunningState& state)
                                    -> std::optional<std::variant<RunningState, ErrorState, FinishedState>> {
                                auto thread_st = state.thread->poll();

                                switch (thread_st) {
                                    case details::thread_state::running:
                                        return std::nullopt;

                                    case details::thread_state::finished: {
                                        //TODO: how to get the value
                                        return FinishedState{};
                                    }
                                    case details::thread_state::aborted:
                                    default: {
                                        return ErrorState{ state.thread->state() };
                                    }
                                }
                            },
                            [this](const ErrorState& state)
                                    -> std::optional<std::variant<RunningState, ErrorState, FinishedState>> {
                                return std::nullopt;
                            },
                            [this](const FinishedState& state)
                                    -> std::optional<std::variant<RunningState, ErrorState, FinishedState>> {
                                return std::nullopt;
                            },

                    },
                    this->m_state
            );

            if (new_value.has_value()) {
                this->m_state = std::move(new_value.value());
            }
        }

    public:
        future(details::SecondaryCPUState* const cpu,
               std::shared_ptr<details::ThreadInfo>&& info,
               std::shared_ptr<details::DetachedThreadStatePublic>&& thread)
            : m_state{
                  RunningState{ cpu, std::move(info), std::move(thread) }
        } {
            //
        }

        template<class _Clock, class _Duration>
        [[nodiscard]] future_status wait_until(const std::chrono::time_point<_Clock, _Duration>& __abs_time) const {

            //TODO: implement other wait too, but we only use this atm
            if (__abs_time != std::chrono::system_clock::time_point::min()) {
                spdlog::error("Invalid abs_time for wait_until: {}", __abs_time);
                abort();
            }

            this->__internal_poll();


            return std::visit(
                    ::helper::uefi::future::Overloaded{
                            [this](const RunningState& state) -> future_status { return future_status::timeout; },
                            [this](const ErrorState& state) -> future_status { return future_status::error; },
                            [this](const FinishedState& state) -> future_status { return future_status::ready; },

                    },
                    this->m_state
            );
        }
    };

    using async_launch_type = std::monostate;


    //NOTE: that all calls in this thread are NOT THREAD  SAFE, but it should be fine(tm), as we don't use many services at a time anyway
    template<class F, class... Args>
    [[nodiscard]] future<std::invoke_result_t<std::decay_t<F>, std::decay_t<Args>...>>
    async(async_launch_type launch_type, F&& f, Args&&... args) {

        // NOTE: atm only implemented for this, so we don't have to pass arguments ore set the result value
        static_assert(sizeof...(Args) == 0, "the function inside async() doesn't support arguments yet");

        static_assert(
                std::is_void_v<std::invoke_result_t<std::decay_t<F>, std::decay_t<Args>...>>,
                "the function inside async() doesn't support a non void return type"
        );

        details::init_cpu_state();

        ASSERT(details::__cpu_state != nullptr);
        auto my_id = details::my_cpu_id();
        if (details::__cpu_state->BspId != my_id) {
            spdlog::error("Invalid start of async in non main CPU: {} != {}", details::__cpu_state->BspId, my_id);
            abort();
        }

        // need no lock, as only the bsp can do that, and it is single threaded
        details::SecondaryCPUState* const cpu_to_execute_on = details::find_cpu_for_new_thread();

        if (cpu_to_execute_on == nullptr) {
            spdlog::error("Can't find CPU to start thread on", details::__cpu_state->BspId, my_id);
            abort();
        }

        details::RunningFunctionInfo fn_info = { f };

        std::shared_ptr<details::ThreadInfo> info = std::make_shared<details::ThreadInfo>(fn_info);

        auto thread = details::start_detached_thread(cpu_to_execute_on, info);

        return future<std::invoke_result_t<std::decay_t<F>, std::decay_t<Args>...>>{ cpu_to_execute_on, std::move(info),
                                                                                     std::move(thread) };
    }
}; // namespace uefi::helper
