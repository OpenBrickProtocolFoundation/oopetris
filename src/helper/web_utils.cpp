

#if defined(__EMSCRIPTEN__)

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

    [[maybe_unused]] std::optional<std::string> get_item_impl(const std::string& key) {

        thread_local const emscripten::val localStorage = get_local_storage();

        emscripten::val value = localStorage.call<emscripten::val>("getItem", emscripten::val{ key });

        if (value.isNull()) {
            return std::nullopt;
        }

        return value.as<std::string>();
    }

    [[maybe_unused]] void set_item_impl(const std::string& key, const std::string& value) {

        thread_local const emscripten::val localStorage = get_local_storage();

        localStorage.call<void>("setItem", emscripten::val{ key }, emscripten::val{ value });
    }

    [[maybe_unused]] void remove_item_impl(const std::string& key) {

        thread_local const emscripten::val localStorage = get_local_storage();

        localStorage.call<void>("removeItem", emscripten::val{ key });
    }

    [[maybe_unused]] void clear_impl() {

        thread_local const emscripten::val localStorage = get_local_storage();

        localStorage.call<void>("clear");
    }

} // namespace

std::optional<std::string> web::LocalStorage::get_item(const std::string& key) {
    // we don't have access to the localStorage in threads (Web workers)
    //TODO: if we are in the main thread, call the impl directly, otherwise use proxying
    (void) (key);
    return std::nullopt;
}

void web::LocalStorage::set_item(const std::string& key, const std::string& value) {
    //TODO:
    (void) (key);
    (void) (value);
}

void web::LocalStorage::remove_item(const std::string& key) {
    //TODO:
    (void) (key);
}

void web::LocalStorage::clear() {
    //TODO:
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


#endif
