# wepoll - epoll for windows

This library implements the
[epoll](http://man7.org/linux/man-pages/man7/epoll.7.html) API for
Windows applications. It attempts to be efficient, and to match the
Linux API and as closely as possible.

## Rationale

Unlike Linux, OS X, and many other operating systems, Windows doesn't
have a good API for receiving socket state notifications. It only
supports the `select` and `WSAPoll` APIs, but they suffer from
well-understood scalability issues. Using I/O completion ports isn't
always practical when software is designed to be cross-platform.

## Features

* poll 100000s of sockets, efficiently
* fully thread safe, multiple threads can poll the same epoll instance
* sockets can be added to multiple epoll sets

## Limitations

* only works on sockets (pipes, TTYs not supported)
* the EPOLLEXCLUSIVE flag isn't supported

## How to use

The library is distributed as a single source file (wepoll.c) and a
single header file (wepoll.h). Compile the .c file as part of your
project, and include the header wherever needed.

## Compatibility

* Requires Windows 7 or higher.
* Can be compiled with recent versions of MSVC and clang.
  GCC (mingw) should work, but is untested.

## Compatibility notes

**TODO**
