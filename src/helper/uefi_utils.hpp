

#pragma once


#if !defined(__UEFI__)
#error "this header is for uefi only"
#endif


#include "manager/service_provider.hpp"

#include "helper/spdlog_wrapper.hpp"
#include <optional>
#include <spdlog/sinks/callback_sink.h>
#include <string>


#include <core/helper/uefi_debug.h>


namespace uefi {

    [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED std::shared_ptr<spdlog::sinks::callback_sink_st> get_debug_sink();

    [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED std::string map_efi_status_to_string(EFI_STATUS status);

    OOPETRIS_GRAPHICS_EXPORTED void platform_init();

    OOPETRIS_GRAPHICS_EXPORTED void platform_exit();

}; // namespace uefi
