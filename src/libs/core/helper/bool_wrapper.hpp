#pragma once


namespace helper {

    template<typename T>
    struct BoolWrapper {
    private:
        bool m_value;
        std::optional<T> additional;

    public:
        BoolWrapper(bool value) : m_value{ value }, additional{ std::nullopt } { }

        BoolWrapper(bool value, const T& additional) : m_value{ value }, additional{ additional } { }

        BoolWrapper(bool value, const std::optional<T>& additional) : m_value{ value }, additional{ additional } { }

        const std::optional<T>& get_additional() const {
            return additional;
        }

        [[nodiscard]] bool has_additional() const {
            return additional.has_value();
        }


        bool is(const T& value) const {
            return value == additional;
        }

        operator bool() const {
            return m_value;
        }
    };

} // namespace helper
