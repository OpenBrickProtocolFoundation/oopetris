

#if defined(__UEFI__)

#include <core/helper/utils.hpp>

#include "./uefi_utils.hpp"

#include <memory>
#include <string>


std::shared_ptr<spdlog::sinks::callback_sink_st> uefi::get_debug_sink() {
    return std::make_shared<spdlog::sinks::callback_sink_st>([](const spdlog::details::log_msg& msg) {
        const std::string message = std::string{ msg.payload.begin(), msg.payload.end() };


        switch (msg.level) {
            case spdlog::level::off:
                return;
            case spdlog::level::trace:
            case spdlog::level::debug:
                EFI_DEBUG((DEBUG_VERBOSE, "%a\n", message.c_str()));
                break;
            case spdlog::level::info:
                EFI_DEBUG((DEBUG_INFO, "%a\n", message.c_str()));
                break;
            case spdlog::level::warn:
                EFI_DEBUG((DEBUG_WARN, "%a\n", message.c_str()));
                break;
            case spdlog::level::err:
            case spdlog::level::critical:
                EFI_DEBUG((DEBUG_ERROR, "HHEERREE  %a\n", message.c_str()));
                break;
            default:
                return;
        }
    });
}

#endif
