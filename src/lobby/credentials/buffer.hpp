

#pragma once

#include <cstring>
#include <string>

#include "helper/windows.hpp"

namespace secret {
    struct Buffer {
    private:
        std::byte* m_data{ nullptr };
        std::size_t m_size;

    public:
        OOPETRIS_GRAPHICS_EXPORTED explicit Buffer(const std::string& data);
        OOPETRIS_GRAPHICS_EXPORTED explicit Buffer(std::byte* data, std::size_t size);
        template<std::size_t A>
        explicit Buffer(std::array<std::byte, A> data) : m_size{ data.size() } {
            if (m_size == 0) {
                return;
            }

            m_data = new std::byte[m_size]; //NOLINT(cppcoreguidelines-owning-memory)
            std::memcpy(this->m_data, data, m_size);
        }

        OOPETRIS_GRAPHICS_EXPORTED ~Buffer();

        OOPETRIS_GRAPHICS_EXPORTED Buffer(const Buffer& other) = delete;
        OOPETRIS_GRAPHICS_EXPORTED Buffer& operator=(const Buffer& other) = delete;

        OOPETRIS_GRAPHICS_EXPORTED Buffer(Buffer&& other) noexcept;
        OOPETRIS_GRAPHICS_EXPORTED Buffer& operator=(Buffer&& other) noexcept = delete;

        [[nodiscard]] std::string as_string() const;

        [[nodiscard]] std::byte* data() const;

        [[nodiscard]] std::size_t size() const;
    };

} // namespace secret
