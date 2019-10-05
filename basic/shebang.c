#if 0
exec ../c99sh "$0" "$@"
#endif
// Dual shebang providing runnable script and valid C program (in
// scripts skip the ../ prefix).  That is, both ./shebang.c and gcc
// shebang.c work.

#include <stdio.h>

int main(int argc, char *argv[])
{
    printf("Hello, world!\n");

    // Print all arguments not including the binary name
    for (int i = 1; i < argc; ++i) {
        puts(argv[i]);
    }

    return 0;
}
