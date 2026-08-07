#include "./compat.hpp"


#if defined(__OOPETRIS_NO_STREAMS)

#include <core/helper/expected.hpp>
#include <fmt/format.h>


static helper::expected<compat::Buffer, std::string> read_file(const char* file_path) {
    FILE* file = fopen(file_path, "rb");

    if (file == NULL) {

        return helper::unexpected<std::string>{
            fmt::format("Couldn't open file for reading '{}': {}", file_path, strerror(errno))
        };
    }

    const int fseek_res = fseek(file, 0, SEEK_END);

    if (fseek_res != 0) {
        return helper::unexpected<std::string>{
            fmt::format("Couldn't seek to end of file '{}': {}", file_path, strerror(errno))
        };
    }

    const long file_size = ftell(file);

    if (file_size < 0) {
        return helper::unexpected<std::string>{ fmt::format("File size is negative: '{}': {}", file_path, file_size) };
    }

    const int fseek_res2 = fseek(file, 0, SEEK_SET);

    if (fseek_res2 != 0) {
        return helper::unexpected<std::string>{
            fmt::format("Couldn't seek to start of file '{}': {}", file_path, strerror(errno))
        };
    }

    uint8_t* file_data = (uint8_t*) malloc((size_t) file_size * sizeof(uint8_t));

    if (!file_data) {
        fclose(file);
        return helper::unexpected<std::string>{ "Couldn't allocate bytes" };
    }

    const size_t fread_result = fread(file_data, 1, (size_t) file_size, file);

    if (fread_result != (size_t) file_size) {
        const char* error_msg = strerror(errno);

        fclose(file);
        free(file_data);

        return helper::unexpected<std::string>{
            fmt::format("Couldn't read the correct amount of bytes from file '{}': {}", file_path, error_msg)
        };
    }

    const int fclose_result = fclose(file);

    if (fclose_result != 0) {
        const char* error_msg = strerror(errno);

        free(file_data);
        return helper::unexpected<std::string>{ fmt::format("Couldn't close file '{}': {}", file_path, error_msg) };
    }

    return compat::Buffer{ file_data, (size_t) file_size };
}


compat::ifstream_basic::ifstream_basic(const std::filesystem::path& path) {
    auto data = read_file(path.string().c_str());

    if (not data.has_value()) {
        m_fail = true;
        return;
    }

    auto ok = data.value();

    m_data = ok;
    m_open = true;
}

compat::ifstream_basic::~ifstream_basic() noexcept {
    free(m_data.data);
}

std::string compat::ifstream_basic::copied_data() {
    return std::string{ (char*)m_data.data, m_data.size };
}

bool compat::ifstream_basic::is_open() const {
    return m_open;
}

bool compat::ifstream_basic::fail() const {
    return m_fail;
}

void compat::ifstream_basic::close() {
    m_open = false;
}

#endif
