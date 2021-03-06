# png-plte

Play with [PNG format](http://www.libpng.org/pub/png/spec/1.2/PNG-Contents.html)



## TLDR - Quick compile

No dependency, just **make** to compile sources and have fun with `./bin/png-plte`

> For the moment I also compile with `libsdl2-dev` (but it's temporary)



## Dependencies

I want **as few dependecies as possible**

However I need [zlib](http://zlib.net/). `lib/makefile` downloads a tar version and compiles the static lib so you do not need to install it. Feel free to edit `ZLIB` variable is `header.mk` to link to different sources.

You do not need all the next packages, just those used for the specific makefile command which I list here

- `make test`: [CUnit](http://cunit.sourceforge.net/index.html) (only `<CUnit/Basic.h>`) linked with `CUNIT=-lcunit`
- `make cov`: [gcov](https://gcc.gnu.org/onlinedocs/gcc/Gcov.html) compiling options are in the variable `COV=-O0 -fprofile-arcs -ftest-coverage`
- `make doc`: [Doxygen](https://www.doxygen.nl/manual/commands.html)

> Compilation constants are in `header.mk`



### Makefiles

I had a raw idea about how I wanted to build my project so I basicaly throw few makefiles in it and fix them until they get reliable.

Here are the basic guidelines I had:
- Outsource building in `bin/`
- Recursive makefiles in `src/`, `lib/` and `tst/`
- Makefile constantes in `header.mk` (included in other makefiles)
- Global targets in `footer.mk` (included in other makefiles as well)

Run `make help` to have target descriptions (it's pretty simple though) 

Notes:
- The first `make test` downloads the folder `tst/suite/` which is a [test-suite for PNG](http://www.schaik.com/pngsuite/) for test only
- `make cov` is really simple but if needed I run `cd bin/ && gcov *.gcno` to see unexecuted lines in the corresponding `.c.gcov` file
- Disable log with `LOG=NONE`, but you have a whole collection of log level in `src/log.h`
  - Take care to compile with `LOG=ALL` in order to try the compiler against every lines of code


## Documentation

As I tend to leave a project for several months straight,
I tough it would be nice to write some documentation using [Doxygen](https://www.doxygen.nl/manual/commands.html)
to easily come back each time.

At least, each header file begins with
```
/**
 * @file
 * @brief
 * @details
 */
```
And each function must be well described in it
```
/**
 * @brief
 * @param[in,out]
 * @return
 */
```
Otherwise `make doc` prints warning about not documented content