#include "utest.h"

#ifdef UNIT_TESTS
#include "m.h"
#elif E2E_TESTS_TWO_FILES
#include "mg.h"
#else
#define MOREWARD_IMPL
#include "moreward.h"
#endif


UTEST(Scratch, Logging) {
    IErr errß = 0;
    m_log("hello %d", 10);
    m_log_info("hello again %d", 10);
    m_log_trace("hello (trace) you shouldn't see this %d", 10);
    m_set_loglevel(M_LOG_TRACE);
    m_log_trace("hello (trace) you see this %d", 10);
}

UTEST_MAIN();