

#pragma once


#if !defined(__UEFI__)
#error "this header is for uefi only"
#endif


#include "manager/service_provider.hpp"

#include "helper/spdlog_wrapper.hpp"
#include <optional>
#include <spdlog/sinks/callback_sink.h>
#include <string>


#include <core/helper/uefi_debug.hpp>


namespace uefi {

    [[nodiscard]] std::shared_ptr<spdlog::sinks::callback_sink_st> get_debug_sink();

}; // namespace uefi
