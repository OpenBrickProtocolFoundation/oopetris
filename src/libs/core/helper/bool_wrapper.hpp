#pragma once


#include <optional>

namespace helper {

    template<typename T>
    struct BoolWrapper {
    private:
        bool m_value;
        std::optional<T> m_additional;

    public:
        BoolWrapper(bool value) //NOLINT(google-explicit-constructor)
            : m_value{ value },
              m_additional{ std::nullopt } { }

        BoolWrapper(bool value, const T& additional) : m_value{ value }, m_additional{ additional } { }

        BoolWrapper(bool value, const std::optional<T>& additional) : m_value{ value }, m_additional{ additional } { }

        const std::optional<T>& get_additional() const {
            return m_additional;
        }

        [[nodiscard]] bool has_additional() const {
            return m_additional.has_value();
        }


        bool is(const T& value) const {
            return value == m_additional;
        }

        operator bool() const { //NOLINT(google-explicit-constructor)
            return m_value;
        }
    };

} // namespace helper
