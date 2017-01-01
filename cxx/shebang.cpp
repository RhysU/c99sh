#if 0
exec ../cxxsh "$0"
#endif
// Dual shebang providing runnable script and valid C++ program (in
// scripts skip the ../ prefix).  That is, both ./shebang.cpp and g++
// shebang.cpp work.

#include <iostream>

using namespace std;

int main()
{
    cout << "Hello, world!" << endl;
}
