#if 0
exec ../c99sh "$0"
#endif
// Dual shebang providing runnable script and valid C program (in
// scripts skip the ../ prefix).  That is, both ./shebang.c and gcc
// shebang.c work.

#include <stdio.h>

int main()
{
    printf("Hello, world!\n");
    return 0;
}
