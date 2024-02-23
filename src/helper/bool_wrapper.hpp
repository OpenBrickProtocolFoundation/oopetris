#include "helper/optional.hpp"


namespace helper {

    template<typename T>
    struct BoolWrapper {
    private:
        bool value;
        helper::optional<T> additional;

    public:
        BoolWrapper(bool value) : value{ value }, additional{ helper::nullopt } { }

        BoolWrapper(bool value, const T& additional) : value{ value }, additional{ additional } { }

        BoolWrapper(bool value, const helper::optional<T>& additional) : value{ value }, additional{ additional } { }

        const helper::optional<T>& get_additional() const {
            return additional;
        }

        bool has_additional() const {
            return additional.has_value();
        }


        bool is(const T& value) const {
            return value == additional;
        }

        operator bool() const {
            return value;
        }
    };

} // namespace helper
