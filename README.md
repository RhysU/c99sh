c99sh
=====

A shebang-friendly script for "interpreting" single C99, C11, and C++ files,
including rcfile support.

For example, installing this `~/.c99shrc` control file

    -Wall -g -O2
    #include <stdio.h>

permits executing [hello](basic/hello) containing

    #!/usr/bin/env c99sh
    int main()
    {
        puts("Hello, world!");
    }

to produce the output one expects provided [c99sh](c99sh) is in the path. You
may also run `c99sh foo.c` to execute some `foo.c` lacking the shebang line. Try
`c99sh -v foo.c` if you encounter trouble and want to see the compilation
command. Check out `c99sh -h` for all the command line options you might use. In
particular, for simple tasks you might find that the command line options in
conjunction with HERE documents can accomplish many things.  For example,

    $ ./c99sh -sm <<HERE
    puts("Hello, world!");
    HERE

Control files can supply compilation and linking flags, preprocessor directives
like `#include`, and
[pkg-config](http://www.freedesktop.org/wiki/Software/pkg-config/) directives to
simplify library usage. A `c99shrc` located in the same directory as the
interpreted source will be used. Otherwise a `~/.c99shrc` is processed if
available. See [c99shrc.example](c99shrc.example) for an extended control file
enabling [GSL](http://www.gnu.org/software/gsl/),
[GLib](https://developer.gnome.org/glib/), and [SQLite](http://www.sqlite.org/)
capabilities.  This permits quickly getting access to higher-level data
structures.

A more entertaining example is an [OpenMP](http://openmp.org/wp/)-enabled Monte
Carlo computation of π screaming like a banshee on all your cores
([c99shrc](openmp/c99shrc), [source](openmp/pi)):

    #!/usr/bin/env c99sh

    int main(int argc, char *argv[])
    {
        long long niter = argc > 1 ? atof(argv[1]) : 100000;
        long long count = 0;

        #pragma omp parallel
        {
            unsigned int seed = omp_get_thread_num();

            #pragma omp for reduction(+: count) schedule(static)
            for (long long i = 0; i < niter; ++i) {
                const double x = rand_r(&seed) / (double) RAND_MAX;
                const double y = rand_r(&seed) / (double) RAND_MAX;
                count += sqrt(x*x + y*y) < 1;
            }

        }

        printf("%lld: %g\n", niter, M_PI - 4*(count / (double) niter));
    }

Take that, [GIL](http://en.wikipedia.org/wiki/Global_Interpreter_Lock).

Kidding aside, the speedup in the edit-compile-run loop can be handy during
prototyping or analysis.  It is nice when useful one-off scripts can be moved
directly into C ABI code instead of requiring an additional
{Python,Octave,R}-to-C translation and debugging phase.  For example, compare
the [Octave version](gsl/nozzle_match.m) of some simple logic with the
[equivalent c99sh-based version](gsl/nozzle_match) requiring only a few
[one-time additions](gsl/c99shrc) to your `~/.c99shrc`.

Dual shebang/compiled support, that is a source file that can be both
interpreted via `./shebang.c` and compiled via `gcc shebang.c`, can most
succinctly be achieved (Thanks [mcandre](http://github.com/mcandre) and
[jtsagata](http://github.com/jtsagata)!) as follows:

    #if 0
    exec c99sh "$0"
    #endif

    #include <stdio.h>

    int main()
    {
        printf("Hello, world!\n");
    }

As nearly the entire C99-oriented implementation works for C++, by invoking
[c99sh](c99sh) through either a copy or symlink named [cxxsh](cxxsh), you can
write C++-based logic.  The relevant control files are named like `cxxshrc` in
this case and they support directives like `using namespace std` and `namespace
fb = foo::bar`.  See [cxx/hello](cxx/hello) and [cxx/cxxshrc](cxx/cxxshrc) for a
hello world C++ example.  One nice use case is hacking atop
[Eigen](http://eigen.tuxfamily.org/) since it provides pkg-config support. That
is, `cxxsh -p eigen3 myprogram` builds and runs a one-off, Eigen-based program.
With the right `cxxshrc`, such a program can be turned into a script.  Though,
you will likely notice the compilation overhead much moreso with C++ than C99.
That said, for repeated invocation an output binary can be saved with the `-x`
option should repeated recompilation be prohibitively expensive.  C11 can be
used via a symlink named [c11sh](c11sh) with control files like `c11shrc`.  See
[cxx/shebang.cpp](cxx/shebang.cpp) for a C++ dual shebang/compiled idiom.

The idea for `c99sh` came from [21st Century
C](http://shop.oreilly.com/product/0636920025108.do)'s section "Compiling C
Programs via Here Document" ([available
online](http://cdn.oreilly.com/oreilly/booksamplers/9781449327149_sampler.pdf))
by [Ben Klemens](http://ben.klemens.org/). Additionally, I wrote it somewhat in
reaction to browsing the C++-ish work by
[elsamuko/cppsh](https://github.com/elsamuko/cppsh).
