#include "utest.h"

#define MOREWARD_H_IMPL
#include "moreward.h"

UTEST_MAIN();

UTEST(smoke_tests_e2e, smoke) {
    Alloc *a = get_default_alloc();
    EXPECT_EQ(a->udata, nil);
}