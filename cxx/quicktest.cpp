#if 0
exec ../cxxsh -t 'test()' "$0"
#endif
// Dual shebang allowing valid C++ source to be quickly tested like a script
// (in scripts skip the ../ prefix).  That is, both ./quicktest.cpp and
// g++ -c quicktest.cpp work.

#include <iostream>

int logic()
{
    return 42;
}

static void test()
{
    using namespace std;
    cout << logic() << endl;
}
