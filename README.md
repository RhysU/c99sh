c99sh
=====

A shebang-friendly script for "interpreting" single C99 files, including
rudimentary rcfile support.

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
may also run `c99sh foo.c` to execute some `foo.c` lacking the shebang line. A
more entertaining example is [OpenMP](http://openmp.org/wp/)-enabled Monte
Carlo screaming like a banshee on all your cores ([c99shrc](openmp/c99shrc),
[source](openmp/pi)).  Take that,
[GIL](http://en.wikipedia.org/wiki/Global_Interpreter_Lock).

Control files can supply compilation and linking flags, preprocessor directives
like `#include`, and
[pkg-config](http://www.freedesktop.org/wiki/Software/pkg-config/) directives
to simplify library usage. A `c99shrc` located in the same directory as the
interpreted source will be used. Otherwise a `~/.c99shrc` is processed if
available. See [c99shrc.example](c99shrc.example) for an extended control file
enabling [GSL](http://www.gnu.org/software/gsl/),
[GLib](https://developer.gnome.org/glib/), and [SQLite](http://www.sqlite.org/)
capabilities.  This permits quickly getting access to higher-level data
structures.

Idea from [21st Century C](http://shop.oreilly.com/product/0636920025108.do) by
[Ben Klemens](http://ben.klemens.org/) with some thoughts lifted from the
C++-ish work by [elsamuko/cppsh](https://github.com/elsamuko/cppsh).

