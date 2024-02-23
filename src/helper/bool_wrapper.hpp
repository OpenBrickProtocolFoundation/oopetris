#pragma once

#include "helper/optional.hpp"


namespace helper {

    template<typename T>
    struct BoolWrapper {
    private:
        bool m_value;
        helper::optional<T> additional;

    public:
        BoolWrapper(bool value) : m_value{ value }, additional{ helper::nullopt } { }

        BoolWrapper(bool value, const T& additional) : m_value{ value }, additional{ additional } { }

        BoolWrapper(bool value, const helper::optional<T>& additional) : m_value{ value }, additional{ additional } { }

        const helper::optional<T>& get_additional() const {
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
