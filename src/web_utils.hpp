

#pragma once

#include <string>
#include <tl/optional.hpp>


#if not defined(__EMSCRIPTEN__)
#error this header is for emscripten only
#endif

namespace utils {


    struct LocalStorage {

        static tl::optional<std::string> [[nodiscard]] get_item(const std::string& key);
        static void set_item(const std::string& key, const std::string& value);
        static void remove_item(const std::string& key);
        static void clear();
    };


}; // namespace utils