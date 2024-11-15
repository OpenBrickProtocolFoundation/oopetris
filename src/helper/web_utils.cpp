

#if defined(__EMSCRIPTEN__)

#include "web_utils.hpp"
#include <emscripten/val.h>
#include <memory>
#include <string>

std::optional<std::string> web::LocalStorage::get_item(const std::string& key) {

    thread_local const emscripten::val localStorage = emscripten::val::global("localStorage");

    emscripten::val value = localStorage.call<emscripten::val>("getItem", emscripten::val{ key });

    if (value.isNull()) {
        return std::nullopt;
    }

    return value.as<std::string>();
}

void web::LocalStorage::set_item(const std::string& key, const std::string& value) {

    thread_local const emscripten::val localStorage = emscripten::val::global("localStorage");

    localStorage.call<void>("setItem", emscripten::val{ key }, emscripten::val{ value });
}

void web::LocalStorage::remove_item(const std::string& key) {

    thread_local const emscripten::val localStorage = emscripten::val::global("localStorage");

    localStorage.call<void>("removeItem", emscripten::val{ key });
}

void web::LocalStorage::clear() {

    thread_local const emscripten::val localStorage = emscripten::val::global("localStorage");

    localStorage.call<void>("clear");
}


void web::console::error(const std::string& message) {
    thread_local const emscripten::val console = emscripten::val::global("console");
    console.call<void>("error", emscripten::val{ message });
}

void web::console::warn(const std::string& message) {
    thread_local const emscripten::val console = emscripten::val::global("console");
    console.call<void>("warn", emscripten::val{ message });
}

void web::console::log(const std::string& message) {
    thread_local const emscripten::val console = emscripten::val::global("console");
    console.call<void>("log", emscripten::val{ message });
}

void web::console::info(const std::string& message) {
    thread_local const emscripten::val console = emscripten::val::global("console");
    console.call<void>("info", emscripten::val{ message });
}

void web::console::debug(const std::string& message) {
    thread_local const emscripten::val console = emscripten::val::global("console");
    console.call<void>("debug", emscripten::val{ message });
}

void web::console::trace(const std::string& message) {
    thread_local const emscripten::val console = emscripten::val::global("console");
    console.call<void>("trace", emscripten::val{ message });
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
