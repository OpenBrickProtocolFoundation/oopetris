


#include "./buffer.hpp"


secret::Buffer::Buffer(const std::string& data) : m_size{ data.size() } {
    if (m_size == 0) {
        return;
    }

    m_data = new std::byte[m_size]; //NOLINT(cppcoreguidelines-owning-memory)
    std::memcpy(this->m_data, data.data(), m_size);
}

secret::Buffer::Buffer(std::byte* data, std::size_t size) : m_size{ size } {
    if (m_size == 0) {
        return;
    }

    m_data = new std::byte[m_size]; //NOLINT(cppcoreguidelines-owning-memory)
    std::memcpy(this->m_data, data, m_size);
}

secret::Buffer::~Buffer() {
    if (m_data != nullptr && m_size != 0) {
        delete[] m_data;
    }
}

secret::Buffer::Buffer(Buffer&& other) noexcept : m_data{ other.m_data }, m_size{ other.m_size } {
    other.m_size = 0;
    other.m_data = nullptr;
}


[[nodiscard]] std::string secret::Buffer::as_string() const {
    return std::string{
        reinterpret_cast<char*>(m_data), //NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
        reinterpret_cast<char*>(m_data   //NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
        ) + m_size //NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic,coreguidelines-pro-type-reinterpret-cast)
    };
}


[[nodiscard]] std::byte* secret::Buffer::data() const {
    return m_data;
}

[[nodiscard]] std::size_t secret::Buffer::size() const {
    return m_size;
}
