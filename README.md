c99sh
=====

A shebang-friendly script for "interpreting" single C99 files, including rcfile
support.

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
may also run `c99sh foo.c` to execute some `foo.c` lacking the shebang line.
Try `c99sh -v foo.c` if you encounter trouble and want to see the compilation
command.

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

A more entertaining example is an [OpenMP](http://openmp.org/wp/)-enabled Monte
Carlo computation of Pi screaming like a banshee on all your cores
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
{Python,Octave,R}-to-C translation and debugging phase.

The idea for `c99sh` was came from [21st Century
C](http://shop.oreilly.com/product/0636920025108.do)'s section "Compiling C
Programs via Here Document" by [Ben Klemens](http://ben.klemens.org/)
([available
online](http://cdn.oreilly.com/oreilly/booksamplers/9781449327149_sampler.pdf)).
Additionally, I wrote it somewhat in reaction to browsing the C++-ish work by
[elsamuko/cppsh](https://github.com/elsamuko/cppsh).
