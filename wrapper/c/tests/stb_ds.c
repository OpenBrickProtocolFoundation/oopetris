

#include <criterion/criterion.h>

#if defined(__GNUC__)
// these tests have a shit-ton of errors xD
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wmissing-braces"
#pragma GCC diagnostic ignored "-Wint-conversion"
#pragma GCC diagnostic ignored "-Wunused-value"
#pragma GCC diagnostic ignored "-Wsign-compare"
#endif


void assert_impl(bool x) {
    cr_assert(x);
}

#define STBDS_UNIT_TESTS
#define STBDS_ASSERT(x) assert_impl(x);


#include "./thirdparty/stb_ds.h"


#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif


Test(StbDs, UniTests) {
    stbds_unit_tests();
}
