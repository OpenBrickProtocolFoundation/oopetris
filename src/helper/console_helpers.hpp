

#pragma once

#if defined(__CONSOLE__)

namespace console {

    void debug_write(const char* text, size_t size);

    inline void debug_print(const char* text) {
        debug_write(text, strlen(text));
    }

    inline void debug_print(const std::string& value) {
        debug_write(value.c_str(), value.size());
    }

    void platform_init();

    void platform_exit();

    [[nodiscard]] bool inMainLoop();


    template<typename Mutex>
    class debug_sink final : public base_sink<Mutex> {
    public:
        explicit debug_sink() { }
        debug_sink(const debug_sink&) = delete;
        debug_sink& operator=(const debug_sink&) = delete;

    protected:
        void sink_it_(const details::log_msg& msg) override {
            memory_buf_t formatted;
            base_sink<Mutex>::formatter_->format(msg, formatted);
            debug_write(formatted.data(), static_cast<std::streamsize>(formatted.size()));
        }

        void flush_() override {
            // noop
        }

        std::ostream& ostream_;
        bool force_flush_;
    };

    using debug_sink_mt = debug_sink<std::mutex>;
    using debug_sink_st = debug_sink<details::null_mutex>;


} // namespace console


#endif
