c99sh
=====

[![Build Status](https://circleci.com/gh/RhysU/c99sh.svg?style=shield)](https://app.circleci.com/pipelines/github/RhysU/c99sh)

<!-- vim-markdown-toc GFM -->
* [Overview](#overview)
* [Simple Tasks](#simple-tasks)
* [Complicated Tasks](#complicated-tasks)
* [Reference](#reference)
* [Compiling Source with a Shebang](#compiling-source-with-a-shebang)
* [C11 and C23](#c11-and-c23)
* [C++](#c)
* [Credits](#credits)

<!-- vim-markdown-toc -->

Overview
--------

`c99sh` shortens the edit-compile-run loop when prototyping by "interpreting"
single C99, C11, C23, and C++ files.  It is
[shebang](https://en.wikipedia.org/wiki/Shebang_(Unix))-friendly and reads
rcfiles.

For example, with this `~/.c99shrc`

    -Wall -g -O2
    #include <stdio.h>

and [c99sh](c99sh) in your path, [hello](basic/hello) runs as expected:

    #!/usr/bin/env c99sh
    int main()
    {
        puts("Hello, world!");
    }

Simple Tasks
------------

Combine options with HERE documents:

    $ c99sh -ms <<HERE
    puts("Hello, world!");
    HERE

Add lines with `-e`.  Unlike Perl's `-e`, standard input is still read:

    $ c99sh -e 'int main()' -e '{}' </dev/null

Run `c99sh foo.c` when `foo.c` has no shebang line.  Add `-v` to see the
compilation command.

Complicated Tasks
-----------------

Rcfiles simplify using libraries with richer data structures.
[c99shrc.example](c99shrc.example) enables
[GSL](http://www.gnu.org/software/gsl/),
[GLib](https://developer.gnome.org/glib/), and [SQLite](http://www.sqlite.org/)
via [pkg-config](http://www.freedesktop.org/wiki/Software/pkg-config/).

One-off scripts can move directly into C ABI code, skipping a
{Python,Octave,R}-to-C translation and debugging phase.  Compare the [Octave
version](gsl/nozzle_match.m) of some simple logic with the [c99sh
version](gsl/nozzle_match), which needs only a few [one-time
additions](gsl/c99shrc) to your `~/.c99shrc`.

A more entertaining [example](openmp/pi) computes π by OpenMP-enabled Monte
Carlo, screaming like a banshee on all your cores.  Its
[c99shrc](openmp/c99shrc) adds `-fopenmp` and `omp.h`:

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

Reference
---------

    $ c99sh -h
    Usage: c99sh [OPTION]... [--] PROGRAM [PROGRAMOPTION]...
      or:  c99sh [OPTION]... [--] -       [PROGRAMOPTION]...
      or:  c99sh [OPTION]... [--]
    Compile c99 PROGRAM, or standard input, and run it supplying [PROGRAMOPTION]...
    If compilation is successful, the exit status is that of PROGRAM.

    Example:
      echo 'puts("Hello, world!");' | c99sh -ms

    Source options:
      -e LINE  Prepend LINE to any input; often used in conjunction with -ms
      -m       Surround input with main(argc, argv) declaration
      -t STMT  Follow input with main(argc, argv) containing STMT;
      -s       Include all standard C, not C++, headers for the language
      -S       Include all standard C++ library headers for the language

    Build options:
      -l LIB   Link to the library LIB
      -p PKG   Make PKG headers and libraries available to PROGRAM via pkg-config(1)
      -F OPT   Add '-OPT' to $CFLAGS when using $CFLAGS during compilation
      -L OPT   Add '-OPT' to $LDFLAGS when using $LDFLAGS during linking
      -W       Enable and enforce warnings; equivalent to -F Wall -F Werror

    Output options:
      -x EXE   Save the compiled executable as EXE instead of running it
      -E       Print generated source to standard output instead of compiling
      -v       Increase verbosity; may be supplied multiple times
      -h       Display this help message

    Rcfile processing:
      -r RC    Load compilation settings from RC suppressing normal rcfile search
      -R       Suppress rcfile loading; equivalent to -r /dev/null

      An rcfile 'c99shrc' controls compilation if present in the same directory
      as PROGRAM, or in the current working directory when processing standard
      input.  Otherwise, if it exists, the file ~/.c99shrc controls compilation.

    Rcfile syntax:
      Each non-blank line must be a // comment, compiler flags, a preprocessor
      directive, a C++ using or namespace directive, a pkg-config request, linker
      flags, or a source, object, or archive file to build alongside PROGRAM.

        // Single-line comment
        -O2 -Wall
        #include <sqlite3.h>
        using std::vector
        namespace fs = std::filesystem
        pkg-config sqlite3
        -L/foo/lib -lfoo -lm
        /bar/extra_source.c
        /bar/libextra.a

Compiling Source with a Shebang
-------------------------------

Three lines let `./shebang.c` run as a script and `gcc shebang.c` compile it:

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

Add `-t` to test valid C source files quickly:

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

C11 and C23
-----------

C11 and C23 can be used via symlinks named [c11sh](c11sh) and [c23sh](c23sh)
with rcfiles like `c11shrc` and `c23shrc`.

C++
---

Invoke [c99sh](c99sh) through a copy or symlink named [cxxsh](cxxsh) to write
C++.  Rcfiles are then named like `cxxshrc` and also accept directives like
`using namespace std` and `namespace fb = foo::bar`.  See
[cxx/hello](cxx/hello) with [cxx/cxxshrc](cxx/cxxshrc) for hello world.  See
[cxx/shebang.cpp](cxx/shebang.cpp) and [cxx/quicktest.cpp](cxx/quicktest.cpp)
for dual shebang/compiled idioms.

[Eigen](http://eigen.tuxfamily.org/) supports pkg-config, so
`cxxsh -p eigen3 myprogram` builds and runs a one-off Eigen program.  The
right `cxxshrc` turns it into a script.  C++ compiles noticeably slower than C.
Save the binary with `-x` when recompiling costs too much.

Credits
-------

`c99sh` grew from "Compiling C Programs via Here Document" in [Ben
Klemens](http://ben.klemens.org/)'s [21st Century
C](http://shop.oreilly.com/product/0636920025108.do).  That section is
[available
online](http://cdn.oreilly.com/oreilly/booksamplers/9781449327149_sampler.pdf).
[elsamuko/cppsh](https://github.com/elsamuko/cppsh) also prompted it.

[mcandre](http://github.com/mcandre) and
[jtsagata](http://github.com/jtsagata) suggested compiling source with a
shebang.  Thank you both.  I did not think three clean lines could do it.

[mattapiroglu](http://github.com/mattapiroglu) added `-e`.
[flipcoder](https://github.com/flipcoder) added `-l`.
[ProducerMatt](https://github.com/ProducerMatt) added `-F` and `-L`.
