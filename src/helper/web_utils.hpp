

#pragma once


#if !defined(__EMSCRIPTEN__)
#error "this header is for emscripten only"
#endif

#if !defined(__EMSCRIPTEN_PTHREADS__)
#error "need emscripten threads support"
#endif

#include "manager/service_provider.hpp"

#include <optional>
#include <spdlog/sinks/callback_sink.h>
#include <spdlog/spdlog.h>
#include <string>
#include <thread>

#include <emscripten/proxying.h>

namespace web {

    struct LocalStorage {
    private:
        ServiceProvider* m_service_provider;

    public:
        explicit LocalStorage(ServiceProvider* service_provider);

        [[nodiscard]] std::optional<std::string> get_item(const std::string& key) const;
        [[nodiscard]] bool set_item(const std::string& key, const std::string& value) const;
        [[nodiscard]] bool remove_item(const std::string& key) const;
        [[nodiscard]] bool clear() const;
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


    struct WebContext {
    private:
        emscripten::ProxyingQueue m_queue;
        std::thread::id m_main_thread_id;
        pthread_t m_main_thread_handle;
        LocalStorage m_local_storage;

    public:
        explicit WebContext(ServiceProvider* service_provider);

        ~WebContext();

        [[nodiscard]] bool is_main_thread() const;

        void do_processing();

        template<typename T>
        [[nodiscard]] auto proxy(std::function<T()>&& func) {
            using ResultType = typename std::conditional_t<std::is_same_v<T, void>, bool, std::optional<T>>;

            if constexpr (std::is_same_v<ResultType, bool>) {

                std::function<void(emscripten::ProxyingQueue::ProxyingCtx)> proxy_func =
                        [func = std::move(func)](auto ctx) {
                            func();
                            ctx.finish();
                        };

                auto successfull = m_queue.proxySyncWithCtx(m_main_thread_handle, proxy_func);

                return successfull;
            } else {
                ResultType result = std::nullopt;

                std::function<void(emscripten::ProxyingQueue::ProxyingCtx)> proxy_func =
                        [&result, func = std::move(func)](auto ctx) {
                            result = func();
                            ctx.finish();
                        };

                auto successfull = m_queue.proxySyncWithCtx(m_main_thread_handle, proxy_func);


                if (not successfull) {
                    result = std::nullopt;
                }

                return result;
            }
        }

        [[nodiscard]] const LocalStorage& local_storage() const;
    };


}; // namespace web
