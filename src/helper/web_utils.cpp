

#if defined(__EMSCRIPTEN__)

#include <core/helper/utils.hpp>

#include "web_utils.hpp"

#include <emscripten/console.h>
#include <emscripten/val.h>
#include <memory>
#include <string>


namespace {

    emscripten::val get_local_storage() {
        thread_local const emscripten::val localStorage = emscripten::val::global("localStorage");
        return localStorage;
    }

    std::optional<std::string> get_item_impl(const std::string& key) {

        thread_local const emscripten::val localStorage = get_local_storage();

        emscripten::val value = localStorage.call<emscripten::val>("getItem", emscripten::val{ key });

        if (value.isNull()) {
            return std::nullopt;
        }

        return value.as<std::string>();
    }

    void set_item_impl(const std::string& key, const std::string& value) {

        thread_local const emscripten::val localStorage = get_local_storage();

        localStorage.call<void>("setItem", emscripten::val{ key }, emscripten::val{ value });
    }

    void remove_item_impl(const std::string& key) {

        thread_local const emscripten::val localStorage = get_local_storage();

        localStorage.call<void>("removeItem", emscripten::val{ key });
    }

    void clear_impl() {

        thread_local const emscripten::val localStorage = get_local_storage();

        localStorage.call<void>("clear");
    }

} // namespace

web::LocalStorage::LocalStorage(ServiceProvider* service_provider) : m_service_provider{ service_provider } { }

//NOTE:we don't have access to the localStorage in threads (Web workers), so if we are in the main thread, we can call the impl directly, otherwise we have to use proxying
std::optional<std::string> web::LocalStorage::get_item(const std::string& key) const {
    if (m_service_provider->web_context().is_main_thread()) {
        return get_item_impl(key);
    }

    auto result = m_service_provider->web_context().proxy<std::optional<std::string>>([key = std::move(key)]() {
        return get_item_impl(key);
    });

    if (not result.has_value()) {
        return std::nullopt;
    }

    return result.value();
}

bool web::LocalStorage::set_item(const std::string& key, const std::string& value) const {
    if (m_service_provider->web_context().is_main_thread()) {
        set_item_impl(key, value);
        return true;
    }

    auto result = m_service_provider->web_context().proxy<void>([key = std::move(key), value = std::move(value)]() {
        set_item_impl(key, value);
    });

    return result;
}

bool web::LocalStorage::remove_item(const std::string& key) const {
    if (m_service_provider->web_context().is_main_thread()) {
        remove_item_impl(key);
        return true;
    }

    auto result = m_service_provider->web_context().proxy<void>([key = std::move(key)]() { remove_item_impl(key); });

    return result;
}

bool web::LocalStorage::clear() const {
    if (m_service_provider->web_context().is_main_thread()) {
        clear_impl();
        return true;
    }

    auto result = m_service_provider->web_context().proxy<void>([]() { clear_impl(); });

    return result;
}


void web::console::error(const std::string& message) {
    emscripten_console_error(message.c_str());
}

void web::console::warn(const std::string& message) {
    emscripten_console_warn(message.c_str());
}

void web::console::log(const std::string& message) {
    emscripten_console_log(message.c_str());
}

void web::console::info(const std::string& message) {
    emscripten_console_log(message.c_str());
}

void web::console::debug(const std::string& message) {
    // NOTE: really the console, but also debug output
    emscripten_dbg(message.c_str());
}

void web::console::trace(const std::string& message) {
    emscripten_console_trace(message.c_str());
}

void web::console::clear() {
    thread_local const emscripten::val console = emscripten::val::global("console");
    console.call<void>("clear");
}

std::shared_ptr<spdlog::sinks::callback_sink_mt> web::get_console_sink() {
    return std::make_shared<spdlog::sinks::callback_sink_mt>([](const spdlog::details::log_msg& msg) {
        const auto message = std::string{ msg.payload.begin(), msg.payload.end() };

        switch (msg.level) {
            case spdlog::level::off:
                return;
            case spdlog::level::trace:
                web::console::trace(message);
                break;
            case spdlog::level::debug:
                web::console::debug(message);
                break;
            case spdlog::level::info:
                web::console::info(message);
                break;
            case spdlog::level::warn:
                web::console::warn(message);
                break;
            case spdlog::level::err:
            case spdlog::level::critical:
                web::console::error(message);
                break;
            default:
                return;
        }
    });
}

web::WebContext::WebContext(ServiceProvider* service_provider)
    : m_queue{},
      m_main_thread_id{ std::this_thread::get_id() },
      m_main_thread_handle{ pthread_self() },
      m_local_storage{ service_provider } { }

web::WebContext::~WebContext() = default;

[[nodiscard]] bool web::WebContext::is_main_thread() const {
    return std::this_thread::get_id() == m_main_thread_id;
}

void web::WebContext::do_processing() {
    ASSERT(is_main_thread() && "can only process in main thread");

    m_queue.execute();
}

[[nodiscard]] const web::LocalStorage& web::WebContext::local_storage() const {
    return m_local_storage;
}

#endif
