c99sh
=====

<!-- vim-markdown-toc GFM -->
* [Basic Idea](#basic-idea)
* [Usage](#usage)
* [Rcfiles](#rcfiles)
* [Shebang Tricks](#shebang-tricks)
* [C++](#c)
* [C11](#c11)
* [C23](#c23)
* [Credits](#credits)

<!-- vim-markdown-toc -->

Basic Idea
----------

A shebang-friendly script for "interpreting" single C99, C11, and C++ files,
including rcfile support.  [![Build
Status](https://circleci.com/gh/RhysU/c99sh.svg?style=shield)](https://app.circleci.com/pipelines/github/RhysU/c99sh)

For example, installing this `~/.c99shrc` rcfile

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

One or more lines can be included using `-e`:

    $ ./c99sh -e 'int main()' -e '{}'

Usually, `-sm` appears alongside `-e`:

    $ ../c99sh -e 'int start = 3;' -sm <<HERE
    if (start == 3) {
        printf("Hello from 1-liner\n");
    } else {
        return 1;
    }
    HERE

Beware quote escaping for `-e` could use some `printf` love.  Patches welcome.

Usage
-----

    $ c99sh -h
    Usage: c99sh [OPTION]... [--] PROGRAM [PROGRAMOPTION]...
     or:   c99sh [OPTION]... [--] -       [PROGRAMOPTION]...
     or:   c99sh [OPTION]... [--]
    Compile c99 PROGRAM, or standard input, and run it supplying [PROGRAMOPTION]...

    Options:
      -e LINE  Prepends LINE to any input; often used in conjunction with -ms
      -h       Display this help message
      -l LIB   Link to the library LIB
      -m       Wrap input in canonical main(argc, argv) declaration
      -p PKG   Make PKG headers and libraries available to PROGRAM via pkg-config(1)
      -r RC    Load compilation settings from RC suppressing normal rcfile search
      -s       Include all standard C, not C++, headers for the language standard
      -t STMT  Append a main(argc, argv) implementation running statement STMT
      -v       Increase verbosity; may be supplied multiple times
      -x EXE   Save a successfully compiled executable as EXE instead of running it
      -F OPT   Add '-OPT' to $CFLAGS when using $CFLAGS during compilation
      -L OPT   Add '-OPT' to $LDFLAGS when using $LDFLAGS during linking
      -R       Suppress rcfile loading; equivalent to -r /dev/null
      -S       Include all standard C++ library headers for the language standard
      -W       Enable and enforce warnings; equivalent to -F Wall -F Werror

    An rcfile 'c99shrc' controls compilation if present in the same directory as
    PROGRAM, or if present in the current working directory when processing standard
    input.  Otherwise, if it exists, the file ~/.c99shrc controls compilation.

    Each non-blank rcfile line must be a // comment, compiler flags, a preprocessor
    directive, a C++ using or namespace directive, a pkg-config request, linker
    flags, or a source, object, or archive file to build alongside PROGRAM.
    For example:

      // Single-line comment
      -O2 -Wall
      #include <sqlite3.h>
      using std::vector
      namespace fs = std::filesystem
      pkg-config sqlite3
      -L/foo/lib -lfoo -lm
      /bar/extra_source.c
      /bar/libextra.a

    If compilation is successful, the exit status is that of PROGRAM.

Rcfiles
-------

Rcfiles can supply compilation and linking flags, preprocessor directives
like `#include`, and
[pkg-config](http://www.freedesktop.org/wiki/Software/pkg-config/) directives to
simplify library usage. A `c99shrc` located in the same directory as the
interpreted source will be used. Otherwise a `~/.c99shrc` is processed if
available. See [c99shrc.example](c99shrc.example) for an extended rcfile
enabling [GSL](http://www.gnu.org/software/gsl/),
[GLib](https://developer.gnome.org/glib/), and [SQLite](http://www.sqlite.org/)
capabilities.  Rcfiles ease accessing libraries with higher-level
data structures.

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

Shebang Tricks
--------------

Dual shebang/compiled support, that is a source file that can be both
interpreted via `./shebang.c` and compiled via `gcc shebang.c`, can most
succinctly be achieved as follows:

    #if 0
    exec c99sh "$0" "$@"
    #endif

    #include <stdio.h>

    int main(int argc, char *argv[])
    {
        for (int i = 1; i < argc; ++i) {
            printf("Hello, %s!\n", argv[i]);
        }
    }

This dual shebang approach permits quick testing/iteration on valid
C source files using the `-t` option:

    #if 0
    exec c99sh -t 'test()' "$0" "$@"
    #endif

    #include <stdio.h>

    int logic()
    {
        return 42;
    }

    static void test()
    {
        printf("%d\n", logic());
    }

Testing in this manner resembles how folks use Python's `__main__` inside
libraries.

C++
---

As nearly the entire C99-oriented implementation works for C++, by invoking
[c99sh](c99sh) through either a copy or symlink named [cxxsh](cxxsh), you can
write C++-based logic.  The relevant rcfiles are named like `cxxshrc` in
this case and they support directives like `using namespace std` and `namespace
fb = foo::bar`.  See [cxx/hello](cxx/hello) and [cxx/cxxshrc](cxx/cxxshrc) for a
hello world C++ example.  See [cxx/shebang.cpp](cxx/shebang.cpp) and
[cxx/quicktest.cpp](cxx/quicktest.cpp) for C++ dual shebang/compiled idioms.

One nice use case is hacking atop [Eigen](http://eigen.tuxfamily.org/) since it
provides pkg-config support. That is, `cxxsh -p eigen3 myprogram` builds and
runs a one-off, Eigen-based program.  With the right `cxxshrc`, such a program
can be turned into a script.  Though, you will likely notice the compilation
overhead much moreso with C++ than C99.  That said, for repeated invocation an
output binary can be saved with the `-x` option should repeated recompilation be
prohibitively expensive.

C11
---

C11 can be used via a symlink named [c11sh](c11sh) with rcfiles like
`c11shrc`.

C23
---

C23 can be used via a symlink named [c23sh](c23sh) with rcfiles like
`c23shrc`.

Credits
-------

The idea for `c99sh` came from [21st Century
C](http://shop.oreilly.com/product/0636920025108.do)'s section "Compiling C
Programs via Here Document" ([available
online](http://cdn.oreilly.com/oreilly/booksamplers/9781449327149_sampler.pdf))
by [Ben Klemens](http://ben.klemens.org/). Additionally, I wrote it somewhat in
reaction to browsing the C++-ish work by
[elsamuko/cppsh](https://github.com/elsamuko/cppsh).

The dual shebang/compiled approach was suggested by
[mcandre](http://github.com/mcandre) and
[jtsagata](http://github.com/jtsagata).  Thank you both for pushing on the
idea, as I did not think it could be done in three clean lines.

The one line execution similar to Perl's -e was done by
[mattapiroglu](http://github.com/mattapiroglu).

The `-l` command line option was contributed by
[flipcoder](https://github.com/flipcoder).

The `-F` and `-L` command line options were contributed by
[ProducerMatt](https://github.com/ProducerMatt).
