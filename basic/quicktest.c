#if 0
exec ../c99sh -t 'test()' "$0"
#endif
// Dual shebang allowing valid C source to be quickly tested like a script
// (in scripts skip the ../ prefix).  That is, both ./quicktest.c and
// gcc -c quicktest.c work.

#include <stdio.h>

int logic()
{
    return 42;
}

static void test()
{
    printf("%d\n", logic());
}
