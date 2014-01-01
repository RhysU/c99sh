c99sh
=====

A shebang-friendly script for "interpreting" single C99 files.
Functionality includes a rudimentary rcfile processor.

For example, using this `~/.c99shrc`

    -Wall -g -O2
    #include <stdio.h>

permits executing [hello](hello) containing

    #!/usr/bin/env c99sh
    int main()
    {
        puts("Hello, world!");
    }

to produce the output one expects provided [c99sh](c99sh) is in the path.

Control files can supply compilation and linking flags, preprocessor directives
like `#include`, and
[pkg-config](http://www.freedesktop.org/wiki/Software/pkg-config/) directives.
See [.c99shrc](.c99shrc) for an extended example enabling
[GSL](http://www.gnu.org/software/gsl/),
[GLib](https://developer.gnome.org/glib/), and [SQLite](http://www.sqlite.org/)
capabilities.

Idea from [21st Century C](http://shop.oreilly.com/product/0636920025108.do) by
[Ben Klemens](http://ben.klemens.org/) with some thoughts lifted
from the C++-ish work by [elsamuko/cppsh](https://github.com/elsamuko/cppsh).
