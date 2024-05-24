

#include "additional_information.hpp"
#include "helper.hpp"

#include <algorithm>
#include <functional>

[[nodiscard]] std::string recorder::InformationValue::to_string(u32 recursion_depth // NOLINT(misc-no-recursion)
) const {
    return std::visit(
            helper::overloaded{
                    [](const std::string& value) { return value; },
                    [](const float& value) { return std::to_string(value); },
                    [](const double& value) { return std::to_string(value); },
                    [](const bool& value) { return std::string{ value ? "true" : "false" }; },
                    [](const u8& value) { return std::to_string(static_cast<int>(value)); },
                    [](const i8& value) { return std::to_string(static_cast<int>(value)); },
                    [](const u32& value) { return std::to_string(value); },
                    [](const i32& value) { return std::to_string(value); },
                    [](const u64& value) { return std::to_string(value); },
                    [](const i64& value) { return std::to_string(value); },
                    [recursion_depth](const std::vector<recorder::InformationValue>& value // NOLINT(misc-no-recursion)
                    ) {
                        if (recursion_depth >= max_recursion_depth) {
                            throw std::runtime_error{ fmt::format(
                                    "Reached maximum recursion depth of {} while printing vectors!", max_recursion_depth
                            ) };
                        }

                        std::vector<std::string> strings{};
                        strings.reserve(value.size());
                        for (const auto& element : value) {
                            strings.push_back(element.to_string(recursion_depth + 1));
                        }

                        return fmt::format("{{ {} }}", fmt::join(strings, ", "));
                    } },
            m_value
    );
}


helper::expected<std::pair<std::string, recorder::InformationValue>, std::string>
recorder::InformationValue::read_from_istream(std::istream& istream) {

    const auto key = read_string_from_istream(istream);
    if (not key.has_value()) {
        return helper::unexpected<std::string>{ key.error() };
    }

    const auto value = read_value_from_istream(istream);
    if (not value.has_value()) {
        return helper::unexpected<std::string>{ value.error() };
    }

    return std::pair<std::string, recorder::InformationValue>{ key.value(), value.value() };
}

[[nodiscard]] helper::expected<std::vector<char>, std::string>
recorder::InformationValue::to_bytes( // NOLINT(misc-no-recursion)
        u32 recursion_depth
) const {
    auto bytes = std::vector<char>{};

    if (is<std::string>()) {
        helper::writer::append_value<std::underlying_type_t<ValueType>>(bytes, std::to_underlying(ValueType::String));
        const auto value = string_to_bytes(as<std::string>());
        helper::writer::append_bytes(bytes, value);
        return bytes;
    }

    if (is<float>()) {
        helper::writer::append_value<std::underlying_type_t<ValueType>>(bytes, std::to_underlying(ValueType::Float));
        static_assert(sizeof(float) == 4 && sizeof(u32) == 4);

        typedef union {
            u32 original_value;
            float value;
        } FloatConversion;

        const FloatConversion conversion_value{ .value = as<float>() };
        helper::writer::append_value<u32>(bytes, conversion_value.original_value);
        return bytes;
    }

    if (is<double>()) {
        helper::writer::append_value<std::underlying_type_t<ValueType>>(bytes, std::to_underlying(ValueType::Double));
        static_assert(sizeof(double) == 8 && sizeof(u64) == 8);

        typedef union {
            u64 original_value;
            double value;
        } DoubleConversion;

        const DoubleConversion conversion_value{ .value = as<double>() };
        helper::writer::append_value<u64>(bytes, conversion_value.original_value);
        return bytes;
    }

    if (is<bool>()) {
        helper::writer::append_value<std::underlying_type_t<ValueType>>(bytes, std::to_underlying(ValueType::Bool));
        static_assert(sizeof(bool) == 1 && sizeof(u8) == 1);
        const auto value = as<bool>();
        helper::writer::append_value<u8>(bytes, static_cast<u8>(value));
        return bytes;
    }

    if (is<u8>()) {
        helper::writer::append_value<std::underlying_type_t<ValueType>>(bytes, std::to_underlying(ValueType::U8));
        static_assert(sizeof(u8) == 1);
        const auto value = as<u8>();
        helper::writer::append_value<u8>(bytes, value);
        return bytes;
    }

    if (is<i8>()) {
        helper::writer::append_value<std::underlying_type_t<ValueType>>(bytes, std::to_underlying(ValueType::I8));
        static_assert(sizeof(i8) == 1);
        const auto value = as<i8>();
        helper::writer::append_value<i8>(bytes, value);
        return bytes;
    }

    if (is<u32>()) {
        helper::writer::append_value<std::underlying_type_t<ValueType>>(bytes, std::to_underlying(ValueType::U32));
        static_assert(sizeof(u32) == 4);
        const auto value = as<u32>();
        helper::writer::append_value<u32>(bytes, value);
        return bytes;
    }

    if (is<i32>()) {
        helper::writer::append_value<std::underlying_type_t<ValueType>>(bytes, std::to_underlying(ValueType::I32));
        static_assert(sizeof(i32) == 4);
        const auto value = as<i32>();
        helper::writer::append_value<i32>(bytes, value);
        return bytes;
    }

    if (is<u64>()) {
        helper::writer::append_value<std::underlying_type_t<ValueType>>(bytes, std::to_underlying(ValueType::U64));
        static_assert(sizeof(u64) == 8);
        const auto value = as<u64>();
        helper::writer::append_value<u64>(bytes, value);
        return bytes;
    }

    if (is<i64>()) {
        helper::writer::append_value<std::underlying_type_t<ValueType>>(bytes, std::to_underlying(ValueType::I64));
        static_assert(sizeof(i64) == 8);
        const auto value = as<i64>();
        helper::writer::append_value<i64>(bytes, value);
        return bytes;
    }

    if (is<std::vector<InformationValue>>()) {
        if (recursion_depth >= max_recursion_depth) {
            return helper::unexpected<std::string>{
                fmt::format("Reached maximum recursion depth of {} while serializing vectors!", max_recursion_depth)
            };
        }

        helper::writer::append_value<std::underlying_type_t<ValueType>>(bytes, std::to_underlying(ValueType::Vector));
        static_assert(sizeof(u32) == 4);

        const auto& vector_value = as<std::vector<InformationValue>>();
        helper::writer::append_value<u32>(bytes, static_cast<u32>(vector_value.size()));

        for (const auto& value : vector_value) {
            const auto value_bytes = value.to_bytes(recursion_depth + 1);
            if (not value_bytes.has_value()) {
                return helper::unexpected<std::string>{
                    fmt::format("Error while converting to bytes: {}", value_bytes.error())
                };
            }

            helper::writer::append_bytes(bytes, value_bytes.value());
        }
        return bytes;
    }

    UNREACHABLE();
}

[[nodiscard]] std::vector<char> recorder::InformationValue::string_to_bytes(const std::string& value) {
    auto bytes = std::vector<char>{};

    static_assert(sizeof(u32) == 4);
    helper::writer::append_value<u32>(bytes, static_cast<u32>(value.size()));

    const char* chars = value.c_str();
    for (decltype(value.size()) i = 0; i < value.size(); ++i) {
        helper::writer::append_value<u8>(bytes, chars[i]); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    }

    return bytes;
}

helper::expected<std::string, std::string> recorder::InformationValue::read_string_from_istream(std::istream& istream) {

    static_assert(sizeof(u32) == 4);
    const auto string_size = helper::reader::read_from_istream<u32>(istream);
    if (not string_size.has_value()) {
        return helper::unexpected<std::string>{ "unable to read string size" };
    }

    const auto size = string_size.value();

    using UniqueCharArray = std::unique_ptr<char, std::function<void(const char* const)>>;

    const UniqueCharArray raw_chars{ new char[size], [](const char* const char_value) {
                                        if (char_value == nullptr) {
                                            return;
                                        }

                                        delete[] char_value; // NOLINT(cppcoreguidelines-owning-memory)
                                    } };
    for (u32 i = 0; i < size; ++i) {

        const auto local_value = helper::reader::read_from_istream<u8>(istream);
        if (not local_value.has_value()) {
            return helper::unexpected<std::string>{ fmt::format("unable to read char in string at index {}", i) };
        }

        raw_chars.get()[i] = static_cast<char>(local_value.value());
    }

    std::string result{ raw_chars.get(), size };

    return result;
}


helper::expected<recorder::InformationValue, std::string>
recorder::InformationValue::read_value_from_istream( // NOLINT(misc-no-recursion)
        std::istream& istream,
        u32 recursion_depth
) {
    const auto magic_byte = helper::reader::read_from_istream<std::underlying_type_t<ValueType>>(istream);
    if (not magic_byte.has_value()) {
        return helper::unexpected<std::string>{ "unable to read magic byte" };
    }

    const auto magic_byte_value = magic_byte.value();

    if (magic_byte_value == utils::to_underlying(ValueType::String)) {
        const auto value = read_string_from_istream(istream);
        if (not value.has_value()) {
            return helper::unexpected<std::string>{ value.error() };
        }
        return recorder::InformationValue{ value.value() };
    }

    if (magic_byte_value == utils::to_underlying(ValueType::Float)) {
        static_assert(sizeof(float) == 4 && sizeof(u32) == 4);
        const auto raw_float = helper::reader::read_from_istream<u32>(istream);
        if (not raw_float.has_value()) {
            return helper::unexpected<std::string>{ "unable to read float value" };
        }

        typedef union {
            u32 original_value;
            float value;
        } FloatConversion;

        const FloatConversion raw_float_value{ .original_value = raw_float.value() };
        return recorder::InformationValue{ raw_float_value.value };
    }

    if (magic_byte_value == utils::to_underlying(ValueType::Double)) {
        static_assert(sizeof(double) == 8 && sizeof(u64) == 8);
        const auto raw_double = helper::reader::read_from_istream<u64>(istream);
        if (not raw_double.has_value()) {
            return helper::unexpected<std::string>{ "unable to read double value" };
        }

        typedef union {
            u64 original_value;
            double value;
        } DoubleConversion;

        const DoubleConversion raw_double_value{ .original_value = raw_double.value() };
        return recorder::InformationValue{ raw_double_value.value };
    }

    if (magic_byte_value == utils::to_underlying(ValueType::Bool)) {

        static_assert(sizeof(bool) == 1 && sizeof(u8) == 1);
        const auto raw_value = helper::reader::read_from_istream<u8>(istream);
        if (not raw_value.has_value()) {
            return helper::unexpected<std::string>{ "unable to read bool value" };
        }
        return recorder::InformationValue{ raw_value.value() != 0 };
    }

    if (magic_byte_value == utils::to_underlying(ValueType::U8)) {

        static_assert(sizeof(u8) == 1);
        const auto raw_value = helper::reader::read_from_istream<u8>(istream);
        if (not raw_value.has_value()) {
            return helper::unexpected<std::string>{ "unable to read u8 value" };
        }
        return recorder::InformationValue{ raw_value.value() };
    }

    if (magic_byte_value == utils::to_underlying(ValueType::I8)) {

        static_assert(sizeof(i8) == 1);
        const auto raw_value = helper::reader::read_from_istream<i8>(istream);
        if (not raw_value.has_value()) {
            return helper::unexpected<std::string>{ "unable to read i8 value" };
        }
        return recorder::InformationValue{ raw_value.value() };
    }

    if (magic_byte_value == utils::to_underlying(ValueType::U32)) {

        static_assert(sizeof(u32) == 4);
        const auto raw_value = helper::reader::read_from_istream<u32>(istream);
        if (not raw_value.has_value()) {
            return helper::unexpected<std::string>{ "unable to read u32 value" };
        }
        return recorder::InformationValue{ raw_value.value() };
    }

    if (magic_byte_value == utils::to_underlying(ValueType::I32)) {

        static_assert(sizeof(i32) == 4);
        const auto raw_value = helper::reader::read_from_istream<i32>(istream);
        if (not raw_value.has_value()) {
            return helper::unexpected<std::string>{ "unable to read i32 value" };
        }
        return recorder::InformationValue{ raw_value.value() };
    }

    if (magic_byte_value == utils::to_underlying(ValueType::U64)) {

        static_assert(sizeof(u64) == 8);
        const auto raw_value = helper::reader::read_from_istream<u64>(istream);
        if (not raw_value.has_value()) {
            return helper::unexpected<std::string>{ "unable to read u64 value" };
        }
        return recorder::InformationValue{ raw_value.value() };
    }

    if (magic_byte_value == utils::to_underlying(ValueType::I64)) {

        static_assert(sizeof(i64) == 8);
        const auto raw_value = helper::reader::read_from_istream<i64>(istream);
        if (not raw_value.has_value()) {
            return helper::unexpected<std::string>{ "unable to read i64 value" };
        }
        return recorder::InformationValue{ raw_value.value() };
    }

    if (magic_byte_value == utils::to_underlying(ValueType::Vector)) {
        if (recursion_depth >= max_recursion_depth) {
            return helper::unexpected<std::string>{
                fmt::format("Reached maximum recursion depth of {} while de-serializing vectors!", max_recursion_depth)
            };
        }

        static_assert(sizeof(u32) == 4);
        const auto vector_size = helper::reader::read_from_istream<u32>(istream);
        if (not vector_size.has_value()) {
            return helper::unexpected<std::string>{ "unable to read vector size" };
        }

        std::vector<InformationValue> result{};
        result.reserve(vector_size.value());
        for (u32 i = 0; i < vector_size.value(); ++i) {

            const auto local_value = read_value_from_istream(istream, recursion_depth + 1);
            if (not local_value.has_value()) {
                return helper::unexpected<std::string>{
                    fmt::format("unable to read value in vector at index {}: {}", i, local_value.error())
                };
            }

            result.push_back(local_value.value());
        }

        return recorder::InformationValue{ result };
    }


    return helper::unexpected<std::string>{
        fmt::format("invalid magic byte: {}", static_cast<int>(magic_byte.value()))
    };
}

recorder::AdditionalInformation::AdditionalInformation(UnderlyingContainer&& values) : m_values{ std::move(values) } { }

recorder::AdditionalInformation::AdditionalInformation() = default;

helper::expected<recorder::AdditionalInformation, std::string> recorder::AdditionalInformation::from_istream(
        std::istream& istream
) {

    const auto magic_bytes =
            helper::reader::read_from_istream<decltype(AdditionalInformation::magic_start_byte)>(istream);
    if (not magic_bytes.has_value()) {
        return helper::unexpected<std::string>{ "unable to read magic file bytes from recorded game" };
    }
    if (magic_bytes.value() != AdditionalInformation::magic_start_byte) {
        return helper::unexpected<std::string>{ "magic start bytes are not correct, the data is probably corrupted" };
    }

    const auto num_pairs = helper::reader::read_from_istream<u32>(istream);
    static_assert(sizeof(decltype(num_pairs.value())) == 4);
    if (not num_pairs.has_value()) {
        return helper::unexpected<std::string>{ "unable to read number of pairs" };
    }

    UnderlyingContainer values{};

    values.reserve(num_pairs.value());
    for (u32 i = 0; i < num_pairs.value(); ++i) {
        auto value_result = InformationValue::read_from_istream(istream);
        if (not value_result.has_value()) {
            return helper::unexpected<std::string>{
                fmt::format("failed to read value from AdditionalInformation: {}", value_result.error())
            };
        }

        const auto& [key, value] = value_result.value();

        if (values.contains(key)) {
            return helper::unexpected<std::string>{
                fmt::format("AdditionalInformation already contains key '{}'", key)
            };
        }

        values.insert_or_assign(key, value);
    }

    auto information = AdditionalInformation{ std::move(values) };

    const auto calculated_checksum = information.get_checksum();
    if (not calculated_checksum.has_value()) {
        return helper::unexpected<std::string>{ "unable to calculate the checksum from AdditionalInformation" };
    }

    const auto read_checksum =
            helper::reader::read_array_from_istream<Sha256Stream::Checksum::value_type, Sha256Stream::ChecksumSize>(
                    istream
            );
    if (not read_checksum.has_value()) {
        return helper::unexpected<std::string>{ "unable to read the checksum from AdditionalInformation" };
    }
    if (read_checksum.value() != calculated_checksum.value()) {
        return helper::unexpected<std::string>{ fmt::format(
                "value checksum mismatch, the AdditionalInformation was altered: expected {:x} but got {:x}",
                fmt::join(calculated_checksum.value(), ""), fmt::join(read_checksum.value(), "")
        ) };
    }

    return information;
}

void recorder::AdditionalInformation::add_value(const std::string& key, const InformationValue& value, bool overwrite) {
    if (m_values.contains(key) and not overwrite) {
        throw std::runtime_error("Can't overwrite already existing key");
    }

    m_values.insert_or_assign(key, value);
}

helper::optional<recorder::InformationValue> recorder::AdditionalInformation::get(const std::string& key) const {

    if (not has(key)) {
        return helper::nullopt;
    }

    return m_values.at(key);
}

[[nodiscard]] bool recorder::AdditionalInformation::has(const std::string& key) const {
    return m_values.contains(key);
}


[[nodiscard]] helper::expected<std::vector<char>, std::string> recorder::AdditionalInformation::to_bytes() const {
    auto bytes = std::vector<char>{};

    static_assert(sizeof(decltype(AdditionalInformation::magic_start_byte)) == 4);
    helper::writer::append_value(bytes, AdditionalInformation::magic_start_byte);

    static_assert(sizeof(u32) == 4);
    helper::writer::append_value(bytes, static_cast<u32>(m_values.size()));

    for (const auto& [key, value] : m_values) {

        const auto key_bytes = InformationValue::string_to_bytes(key);
        helper::writer::append_bytes(bytes, key_bytes);

        const auto value_bytes = value.to_bytes();
        if (not value_bytes.has_value()) {
            return helper::unexpected<std::string>{
                fmt::format("Error while getting bytes for value: {}", value_bytes.error())
            };
        }

        helper::writer::append_bytes(bytes, value_bytes.value());
    }

    const auto checksum = get_checksum();
    if (not checksum.has_value()) {
        return helper::unexpected<std::string>{ fmt::format("Error while getting checksum: {}", checksum.error()) };
    }

    static_assert(sizeof(decltype(checksum.value())) == 32);

    for (const auto& checksum_byte : checksum.value()) {
        helper::writer::append_value<u8>(bytes, checksum_byte);
    }

    return bytes;
}

[[nodiscard]] helper::expected<Sha256Stream::Checksum, std::string> recorder::AdditionalInformation::get_checksum(
) const {
    Sha256Stream sha256_creator{};

    static_assert(sizeof(u32) == 4);
    sha256_creator << static_cast<u32>(m_values.size());

    std::vector<std::string> keys{};
    keys.reserve(m_values.size());
    for (const auto& [key, _] : m_values) {
        keys.push_back(key);
    }

    std::sort(keys.begin(), keys.end());

    for (const auto& key : keys) {
        sha256_creator << InformationValue::string_to_bytes(key);
        const auto bytes = m_values.at(key).to_bytes();
        if (not bytes.has_value()) {
            return helper::unexpected<std::string>{ bytes.error() };
        }

        sha256_creator << bytes.value();
    }

    return sha256_creator.get_hash();
}


recorder::AdditionalInformation::iterator recorder::AdditionalInformation::begin() {
    return m_values.begin();
}

recorder::AdditionalInformation::const_iterator recorder::AdditionalInformation::begin() const {
    return m_values.begin();
}

recorder::AdditionalInformation::iterator recorder::AdditionalInformation::end() {
    return m_values.end();
}

recorder::AdditionalInformation::const_iterator recorder::AdditionalInformation::end() const {
    return m_values.end();
}
