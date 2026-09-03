// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#ifndef SPDLOG_COMPILED_LIB
#error Please define SPDLOG_COMPILED_LIB to compile this file.
#endif

#include <spdlog/common.h>

#include <spdlog/sinks/ansicolor_sink-inl.h>
#include <spdlog/sinks/base_sink-inl.h>

#include <mutex>

#include "./uefi_utils.hpp"

// instantiate the sink template for this mutex
template class SPDLOG_API spdlog::sinks::base_sink<uefi::helper::ThreadMutex>;

template class SPDLOG_API spdlog::sinks::ansicolor_stdout_sink<uefi::spdlog_helper::details::console_mutex>;
template class SPDLOG_API spdlog::sinks::ansicolor_stderr_sink<uefi::spdlog_helper::details::console_mutex>;
