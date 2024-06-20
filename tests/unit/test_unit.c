#include "utest.h"

#ifdef TEST_UNIT
#include "mg.h"
#endif

UTEST_MAIN();

UTEST(smoke_tests_unit, smoke) {
    Alloc *a = get_default_alloc();
    EXPECT_EQ(a->udata, nil);
}

