#ifndef  _TESTS_H_
#define  _TESTS_H_

#ifdef UNIT_TESTS
#include "m.h"
#elif E2E_TESTS_TWO_FILES
#include "mg.h"
#else
#include "moreward.h"
#endif


#endif //  _TESTS_H_
