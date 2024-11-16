

#pragma once


#if !defined(__EMSCRIPTEN__)
#error "this header is for emscripten only"
#endif

#if !defined(__EMSCRIPTEN_PTHREADS__)
#error "need emscripten threads support"
#endif


#include <optional>
#include <spdlog/sinks/callback_sink.h>
#include <spdlog/spdlog.h>
#include <string>

namespace web {

    struct LocalStorage {

        [[nodiscard]] static std::optional<std::string> get_item(const std::string& key);
        static void set_item(const std::string& key, const std::string& value);
        static void remove_item(const std::string& key);
        static void clear();
    };

    [[nodiscard]] std::shared_ptr<spdlog::sinks::callback_sink_mt> get_console_sink();


    namespace console {
        void clear();
        //TODO, these support more arguments, write templates for that
        void error(const std::string& message);
        void warn(const std::string& message);
        void log(const std::string& message);
        void info(const std::string& message);
        void debug(const std::string& message);
        void trace(const std::string& message);
    }; // namespace console


}; // namespace web
