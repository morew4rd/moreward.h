#include "utest.h"

#ifdef TEST_UNIT
#include "m.h"
#else
#include "moreward.h"
#endif

UTEST(smoke_tests_shared, smoke) {
    Alloc *a = get_default_alloc();
    EXPECT_EQ(a->udata, nil);
}