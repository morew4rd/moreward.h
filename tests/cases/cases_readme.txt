Note: These tests are shared between unit and e2e test suites.

Just do this on top of each file:

```

#include "utest.h"

#ifdef TEST_UNIT
#include "m.h"
#else
#include "moreward.h"
#endif

```

And update CmakeLists.txt file if a new file is added.
