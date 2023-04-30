
#if defined(__EMSCRIPTEN__)
#include "web_utils.hpp"
#include <emscripten.h>
#include <string>
#include <tl/optional.hpp>

tl::optional<std::string> utils::LocalStorage::get_item(const std::string& key) {


    thread_local const emscripten::val localStorage = emscripten::val::global("localStorage");

    emscripten::val value localStorage.getItem(key.c_str());

    if (value.isUndefined()) {
        return tl::nullopt;
    }

    return value.as<std::string | undefined>();
}

void utils::LocalStorage::set_item(const std::string& key, const std::string& value) {

    thread_local const emscripten::val localStorage = emscripten::val::global("localStorage");

    localStorage.setItem(key.c_str(), value.c_str());
}

void utils::LocalStorage::remove_item(const std::string& key) {

    thread_local const emscripten::val localStorage = emscripten::val::global("localStorage");

    localStorage.removeItem(key.c_str());
}

void utils::LocalStorage::clear() {

    thread_local const emscripten::val localStorage = emscripten::val::global("localStorage");

    localStorage.clear();
}


#endif