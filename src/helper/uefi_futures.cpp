#include "./uefi_futures.hpp"


extern "C" {
#include <Library/UefiBootServicesTableLib.h>
}

details::MainCPUState* details::__cpu_state = nullptr;


static UINTN my_cpu_id_impl(EFI_MP_SERVICES_PROTOCOL* Mp) {

    ASSERT(Mp);

    UINTN BspNumber;

    EFI_STATUS Status = Mp->WhoAmI(Mp, &BspNumber);

    if (EFI_ERROR(Status)) {
        EFI_DEBUG((DEBUG_ERROR, "ERROR: couldn't execute WhoAmI: %r\n", Status));
        abort();
    }

    return BspNumber;
}


UINTN details::my_cpu_id() {
    ASSERT(__cpu_state != nullptr);
    return my_cpu_id_impl(__cpu_state->Mp);
}


void details::init_cpu_state() {
    if (__cpu_state != nullptr) {
        return;
    }

    __cpu_state = new details::MainCPUState(nullptr, 0, 0, {});


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

        __cpu_state->cpus.emplace_back(info.ProcessorId, (UINTN) i, info.StatusFlag, false);
    }
}


static __attribute__((destructor)) void deinit_cpu_state(void) {
    if (details::__cpu_state == nullptr) {
        return;
    }

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

static void EFIAPI __impl_uefi_new_thread_function(IN OUT VOID* private_data) {
    details::ThreadInfo* ptr = static_cast<details::ThreadInfo*>(private_data);

    ptr->info.fn();
}


struct details::DetachedThreadStateImpl {
    // inout state
    EFI_EVENT DoneEvent;
    BOOLEAN finished;
    //output state
    bool terminated;

    // mutex to protect state, that can be used by the AP (when writing the result) and the BSP (when using poll)
    details::helper::ThreadMutex data_mutex;

    DetachedThreadStateImpl() : DoneEvent{}, finished{ FALSE }, terminated{ false }, data_mutex{} {
        //
    }

    void terminate(void) {
        const std::lock_guard<details::helper::ThreadMutex> scope_lock(this->data_mutex);

        this->terminated = true;
    }

    details::thread_state poll(void) {
        const std::lock_guard<details::helper::ThreadMutex> scope_lock(this->data_mutex);

        // likely for long running async
        if (!this->terminated) [[likely]] {
            return details::thread_state::running;
        }

        // unlikely, as we don't set a timeout
        if (!this->finished) [[unlikely]] {
            return details::thread_state::aborted;
        }

        return details::thread_state::finished;
    }

    ~DetachedThreadStateImpl() noexcept {
        gBS->CloseEvent(DoneEvent);
    }
};

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


static VOID EFIAPI __impl_uefi_thread_done_function(IN EFI_EVENT Event, IN VOID* Context) {
    details::DetachedThreadStateImpl* state = static_cast<details::DetachedThreadStateImpl*>(Context);

    state->terminate();
}


std::shared_ptr<details::DetachedThreadStatePublic> start_detached_thread_impl(
        EFI_MP_SERVICES_PROTOCOL* Mp,
        const details::SecondaryCPUState* const cpu_to_execute_on,
        const std::shared_ptr<details::ThreadInfo>& info
) {

    ASSERT(Mp != nullptr);
    ASSERT(cpu_to_execute_on != nullptr);

    details::DetachedThreadStateImpl* state = new details::DetachedThreadStateImpl();

    EFI_STATUS Status = gBS->CreateEvent(
            EVT_NOTIFY_SIGNAL, TPL_CALLBACK, __impl_uefi_thread_done_function, state, &(state->DoneEvent)
    );

    if (EFI_ERROR(Status)) {
        EFI_DEBUG((DEBUG_ERROR, "ERROR: couldn't execute CreateEvent: %r\n", Status));
        abort();
    }


    Status = Mp->StartupThisAP(
            Mp, __impl_uefi_new_thread_function, cpu_to_execute_on->Id, state->DoneEvent, 0, info.get(),
            &(state->finished)
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
        const details::SecondaryCPUState* const cpu_to_execute_on,
        const std::shared_ptr<details::ThreadInfo>& info
) {

    ASSERT(__cpu_state != nullptr);
    return start_detached_thread_impl(__cpu_state->Mp, cpu_to_execute_on, info);
}
