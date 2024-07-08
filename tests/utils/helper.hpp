

#pragma once

#include "core/helper/utils.hpp"
#include "printer.hpp"

#include <gmock/gmock.h>


MATCHER(ExpectedHasValue, "expected has value") {
    return arg.has_value();
}

MATCHER(ExpectedHasError, "expected has error") {
    return not arg.has_value();
}

MATCHER(OptionalHasValue, "optional has value") {
    return arg.has_value();
}

MATCHER(OptionalHasNoValue, "optional has no value") {
    return not arg.has_value();
}


#define ASSERT_FAIL(x) /*NOLINT(cppcoreguidelines-macro-usage)*/                                 \
    do {                                                                                         \
        std::cerr << "assertion fail in " << __FILE__ << ":" << __LINE__ << ": " << (x) << "\n"; \
        utils::unreachable();                                                                    \
    } while (false)
