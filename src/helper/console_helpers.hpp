

#pragma once

#if defined(__CONSOLE__)

#include <spdlog/sinks/base_sink.h>

namespace console {

    void debug_write(const char* text, size_t size);

    inline void debug_print(const char* text) {
        debug_write(text, strlen(text));
    }

    inline void debug_print(const std::string& value) {
        debug_write(value.c_str(), value.size());
    }

    [[nodiscard]] std::string open_url(const std::string& url);

    void platform_init();

    void platform_exit();

    [[nodiscard]] bool inMainLoop();


    template<typename Mutex>
    class debug_sink final : public spdlog::sinks::base_sink<Mutex> {
    public:
        explicit debug_sink() { }
        debug_sink(const debug_sink&) = delete;
        debug_sink& operator=(const debug_sink&) = delete;

    protected:
        void sink_it_(const spdlog::details::log_msg& msg) override {
            spdlog::memory_buf_t formatted;
            spdlog::sinks::base_sink<Mutex>::formatter_->format(msg, formatted);

            auto size = formatted.size();

#if !defined(__WII__)
            // debug_write already writes an newline at the end (only on 3ds and switch)
            if (size > 0 and formatted.data()[size - 1] == '\n') {
                formatted.data()[size - 1] = '\0';
                --size;
            }
#endif

            debug_write(formatted.data(), size);
        }

        void flush_() override {
            // noop
        }
    };

    using debug_sink_mt = debug_sink<std::mutex>;
    using debug_sink_st = debug_sink<spdlog::details::null_mutex>;


} // namespace console


#endif
