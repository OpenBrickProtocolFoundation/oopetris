#pragma once

#if defined(__OOPETRIS_NO_STREAMS)

#include "./types.hpp"

#include <filesystem>


namespace compat {

    struct Buffer {
        uint8_t* data;
        size_t size;
    };

    class ifstream {
    public:
        explicit ifstream(const std::filesystem::path& path);

        bool is_open() const;
        bool fail() const;
        void close();

        std::string copied_data();

        ~ifstream() noexcept;

    private:
        bool m_open = false;
        bool m_fail = false;

        Buffer m_data;
    };

} // namespace compat


#endif
