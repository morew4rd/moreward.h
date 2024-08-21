#define MOREWARD_IMPL
#include "moreward.h"
#include <stdio.h>


void str_simple(void) {
    m_StrBuffer sb = {0};

}

I32 main(I32 argc, Str *argv) {
    m_StrBuffer sb = {0};
    for (I32 i=0; i<argc; i+=1) {
        ms_cat(&sb, "[arg %d: %s] ", i, argv[i]);
    }
    printf("%s\n", ms_getstr(&sb, NULL));
    return 0;
}