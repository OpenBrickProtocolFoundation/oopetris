#include "./uefi_futures.hpp"


extern "C" {
#include <Library/BaseLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <sys/threads.h>
}

details::MainCPUState* details::__cpu_state = nullptr;


static UINTN my_cpu_id_impl(EFI_MP_SERVICES_PROTOCOL* Mp) {

    ASSERT(Mp);

    UINTN CpuNUmber;

    EFI_STATUS Status = Mp->WhoAmI(Mp, &CpuNUmber);

    if (EFI_ERROR(Status)) {
        EFI_DEBUG((DEBUG_ERROR, "ERROR: couldn't execute WhoAmI: %r\n", Status));
        abort();
    }

    return CpuNUmber;
}


UINTN details::my_cpu_id() {
    ASSERT(__cpu_state != nullptr);
    return my_cpu_id_impl(__cpu_state->Mp);
}

static details::SecondaryCPUState* __impl_get_cpu_state_by_id(UINTN cpu_id) {

    // likely this is 1 indexed into the cpu array, if it is not, than search the whole array

    if (cpu_id == 0) {
        goto search_cpu_in_array;
    }

    if ((cpu_id - 1) < details::__cpu_state->cpus.size()) {
        details::SecondaryCPUState* state = &(details::__cpu_state->cpus.data()[cpu_id - 1]);
        if (state->Id == cpu_id) {
            return state;
        }
        goto search_cpu_in_array;
    }

search_cpu_in_array:

    for (size_t i = 0; i < details::__cpu_state->cpus.size(); ++i) {
        details::SecondaryCPUState* state = &(details::__cpu_state->cpus.data()[i]);
        if (state->Id == cpu_id) {
            return state;
        }
    }

    return nullptr;
}

#define UEFI_THREAD_ABORT_SIGNAL_HANDLER __impl_uefi_thread_abort_signal_handler

static void __impl_uefi_thread_abort_signal_handler(int actual_signal) {
    //TODO: assert actual_signal == SIGABRT

    const std::lock_guard<std::mutex> scope_lock(details::__cpu_state->signal_state.mutex);


    UINTN cpu_id = details::my_cpu_id();

    if (cpu_id == details::__cpu_state->BspId) {
        // don't intercept that on the bsp thread
        return;
    }


    // if this thread is an AP that has a signal_state, don't return, but longjmp to the saved function, where we handle that error

    details::SecondaryCPUState* const state = __impl_get_cpu_state_by_id(cpu_id);

    if (state == nullptr) {
        // nothing found, just abort it
        return;
    }

    if (state->jump_state.has_value()) {
        auto& jump_state = state->jump_state.value().jump_state;

        longjmp(jump_state, 0x42); // Get out of here.  longjmp can't return 0. Use 0x42 for a non-zero value.

        __builtin_unreachable();
    }

    // no signal state for that thread, so continue
    return;
}


void details::init_cpu_state() {
    if (__cpu_state != nullptr) {
        return;
    }

    __cpu_state = new details::MainCPUState(
            nullptr, 0, 0, std::vector<SecondaryCPUState>{}, GlobalSignalState{ nullptr, {} }
    );

    efi_threads_init();


    if (!gBS) {
        EFI_DEBUG((DEBUG_ERROR, "ERROR: gBS is NULL\n"));
        abort();
    }


    EFI_STATUS Status = gBS->LocateProtocol(&gEfiMpServiceProtocolGuid, NULL, (VOID**) &(__cpu_state->Mp));

    if (EFI_ERROR(Status)) {
        EFI_DEBUG((DEBUG_ERROR, "ERROR: couldn't get the EfiMpServiceProtocolGuid: %r\n", Status));
        abort();
    }

    UINTN NumberOfEnabledProcessors;

    Status = __cpu_state->Mp->GetNumberOfProcessors(
            __cpu_state->Mp, &(__cpu_state->NumberOfProcessors), &NumberOfEnabledProcessors
    );

    if (EFI_ERROR(Status)) {
        EFI_DEBUG((DEBUG_ERROR, "ERROR: couldn't get the Number Of Processors: %r\n", Status));
        abort();
    }

    __cpu_state->BspId = my_cpu_id_impl(__cpu_state->Mp);

    //TODO: this is global, so just reset it once, so when old_sig_handler == __impl_sigabrt_handler, don't add it, have one global old handler
    __sighandler_t* old_sig_handler = signal(SIGABRT, UEFI_THREAD_ABORT_SIGNAL_HANDLER);

    //TODO: inline this in start_detached_thread

    __cpu_state->signal_state.old_sig_handler = old_sig_handler;

    __cpu_state->cpus = std::vector<SecondaryCPUState>{};
    __cpu_state->cpus.reserve(__cpu_state->NumberOfProcessors - 1);

    for (size_t i = 0; i < __cpu_state->NumberOfProcessors; ++i) {

        EFI_PROCESSOR_INFORMATION info;

        Status = __cpu_state->Mp->GetProcessorInfo(__cpu_state->Mp, (UINTN) i, &info);

        if (EFI_ERROR(Status)) {
            EFI_DEBUG((DEBUG_ERROR, "ERROR: couldn't Get Processor Info: %r\n", Status));
            abort();
        }

        if ((info.StatusFlag & PROCESSOR_AS_BSP_BIT) != 0) {
            continue;
        }

        __cpu_state->cpus.emplace_back(info.ProcessorId, (UINTN) i, info.StatusFlag, false, std::nullopt);
    }
}

static void __destroy_cpu_state_impl() {
    __sighandler_t* old_sig_handler = signal(SIGABRT, details::__cpu_state->signal_state.old_sig_handler);
    ASSERT(old_sig_handler == UEFI_THREAD_ABORT_SIGNAL_HANDLER);
}


static __attribute__((destructor)) void deinit_cpu_state(void) {
    if (details::__cpu_state == nullptr) {
        return;
    }

    __destroy_cpu_state_impl();

    delete details::__cpu_state;
}


details::SecondaryCPUState* find_cpu_for_new_thread_impl(details::MainCPUState* const state) {

    ASSERT(state);

    for (size_t i = 0; i < state->cpus.size(); i++) {
        details::SecondaryCPUState* cpu = &(state->cpus.data()[i]);

        if (cpu->IsBusy) {
            continue;
        }

        if ((cpu->StatusFlags & PROCESSOR_ENABLED_BIT) == 0) {
            continue;
        }

        if ((cpu->StatusFlags & PROCESSOR_HEALTH_STATUS_BIT) == 0) {
            continue;
        }

        return cpu;
    }

    return nullptr;
}


details::SecondaryCPUState* details::find_cpu_for_new_thread(void) {

    ASSERT(__cpu_state != nullptr);
    return find_cpu_for_new_thread_impl(__cpu_state);
}

struct ThreadLocalState {

    ThreadLocalState() {
        //TODO, add some constructores
    }

    ThreadLocalState(const ThreadLocalState& other) = delete;
    ThreadLocalState& operator=(const ThreadLocalState& other) = delete;

    ThreadLocalState(ThreadLocalState&& other) noexcept = delete;
    ThreadLocalState& operator=(ThreadLocalState&& other) noexcept = delete;


    ~ThreadLocalState() {
        //TODO: add some destructors
    }
};

struct details::DetachedThreadStateImpl {
    // input state
    EFI_EVENT DoneEvent;
    BOOLEAN finished;
    //output state
    std::pair<bool, std::optional<std::string>> run_state;
    // thread info state, not owned
    ThreadInfo* info_ref;
    details::SecondaryCPUState* cpu_to_execute_on_ref;

    // mutex to protect state, that can be used by the AP (when writing the result) and the BSP (when using poll)
    std::mutex data_mutex;

    DetachedThreadStateImpl(ThreadInfo* info_ref, details::SecondaryCPUState* cpu_to_execute_on_ref)
        : DoneEvent{},
          finished{ FALSE },
          run_state{ false, std::nullopt },
          info_ref{ info_ref },
          cpu_to_execute_on_ref{ cpu_to_execute_on_ref },
          data_mutex{} {
        //
    }

    void terminate(std::optional<std::string> error) {
        const std::lock_guard<std::mutex> scope_lock(this->data_mutex);

        this->run_state = { true, error };
    }

    void terminate_done_cb() {
        this->run_state = { true, this->finished ? std::optional<std::string>{ std::nullopt }
                                                 : std::optional<std::string>{ "Not finished" } };
    }

    details::thread_state poll(void) {
        const std::lock_guard<std::mutex> scope_lock(this->data_mutex);

        if (this->run_state.first) {
            return details::thread_state::running;
        }

        if (this->run_state.second.has_value()) {
            return details::thread_state::aborted;
        }

        return details::thread_state::finished;
    }

    std::string state() const {
        return this->run_state.second.has_value() ? this->run_state.second.value() : "<No error>";
    }

    ~DetachedThreadStateImpl() noexcept {
        gBS->CloseEvent(DoneEvent);
    }
};

static void EFIAPI __impl_uefi_new_thread_function(IN OUT VOID* private_data) {

    ThreadLocalState constructors{};

    details::DetachedThreadStateImpl* state = static_cast<details::DetachedThreadStateImpl*>(private_data);

    {

        state->cpu_to_execute_on_ref->jump_state = details::CpuJumpState{};
        auto& setjmp_state = state->cpu_to_execute_on_ref->jump_state.value().jump_state;

        if (setjmp(setjmp_state) == 0) {
            // we are executing it the first time
            state->info_ref->info.fn();
        } else {
            // we aborted
            state->terminate("Thread aborted");
        }

        // in both cases destroy the state
        state->cpu_to_execute_on_ref->jump_state = std::nullopt;
    }
}


details::DetachedThreadStatePublic::DetachedThreadStatePublic(DetachedThreadState* state_impl)
    : m_state_impl{ state_impl } {
    //
}

details::DetachedThreadStatePublic::~DetachedThreadStatePublic() {
    if (m_state_impl != nullptr) {
        delete m_state_impl;
        m_state_impl = nullptr;
    }
}

details::thread_state details::DetachedThreadStatePublic::poll() {
    ASSERT(m_state_impl != nullptr);
    return m_state_impl->poll();
}

std::string details::DetachedThreadStatePublic::state() const {
    ASSERT(m_state_impl != nullptr);
    return m_state_impl->state();
}


static VOID EFIAPI __impl_uefi_thread_done_function(IN EFI_EVENT Event, IN VOID* Context) {
    details::DetachedThreadStateImpl* state = static_cast<details::DetachedThreadStateImpl*>(Context);

    state->terminate_done_cb();
}


std::shared_ptr<details::DetachedThreadStatePublic> start_detached_thread_impl(
        EFI_MP_SERVICES_PROTOCOL* Mp,
        details::SecondaryCPUState* const cpu_to_execute_on,
        const std::shared_ptr<details::ThreadInfo>& info
) {

    ASSERT(Mp != nullptr);
    ASSERT(cpu_to_execute_on != nullptr);

    details::DetachedThreadStateImpl* state = new details::DetachedThreadStateImpl(info.get(), cpu_to_execute_on);

    EFI_STATUS Status = gBS->CreateEvent(
            EVT_NOTIFY_SIGNAL, TPL_CALLBACK, __impl_uefi_thread_done_function, state, &(state->DoneEvent)
    );

    if (EFI_ERROR(Status)) {
        EFI_DEBUG((DEBUG_ERROR, "ERROR: couldn't execute CreateEvent: %r\n", Status));
        abort();
    }


    Status = Mp->StartupThisAP(
            Mp, __impl_uefi_new_thread_function, cpu_to_execute_on->Id, state->DoneEvent, 0, state, &(state->finished)
    );


    if (EFI_ERROR(Status)) {
        EFI_DEBUG((DEBUG_ERROR, "ERROR: couldn't execute StartupThisAP: %r\n", Status));
        abort();
    }

    std::shared_ptr<details::DetachedThreadStatePublic> public_state =
            std::make_shared<details::DetachedThreadStatePublic>(state);

    return public_state;
}


std::shared_ptr<details::DetachedThreadStatePublic> details::start_detached_thread(
        details::SecondaryCPUState* const cpu_to_execute_on,
        const std::shared_ptr<details::ThreadInfo>& info
) {

    ASSERT(__cpu_state != nullptr);
    return start_detached_thread_impl(__cpu_state->Mp, cpu_to_execute_on, info);
}
