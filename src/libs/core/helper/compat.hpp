#pragma once

#if defined(__OOPETRIS_NO_STREAMS)

#include "./types.hpp"

#include <filesystem>


namespace compat {

    struct Buffer {
        uint8_t* data;
        size_t size;
    };

    class ifstream_basic {
    public:
        explicit ifstream_basic(const std::filesystem::path& path);

        bool is_open() const;
        bool fail() const;
        void close();

        std::string copied_data();

        ~ifstream_basic() noexcept;

    private:
        bool m_open = false;
        bool m_fail = false;

        Buffer m_data;
    };


    class ifstream_seekable {
    public:
        explicit ifstream_seekable(const std::filesystem::path& path);

        bool is_open() const;
        bool fail() const;
        void close();

        ifstream_seekable& read(char* s, size_t count);

    private:
        FILE* m_file;
    };


    class ofstream {
    public:
    private:
    };


} // namespace compat


#endif
