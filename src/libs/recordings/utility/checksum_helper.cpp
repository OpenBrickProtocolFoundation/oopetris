

#include "./checksum_helper.hpp"

Sha256Stream::Sha256Stream() = default;

Sha256Stream& Sha256Stream::operator<<(const std::string& value) {

    library_object.add(
            reinterpret_cast<const void*>(value.c_str()), // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
            static_cast<usize>(value.size())
    );
    return *this;
}

[[nodiscard]] Sha256Stream::Checksum Sha256Stream::get_hash() {
    Checksum buffer{};

    library_object.getHash(buffer.data());

    return buffer;
}
