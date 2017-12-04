# wepoll - epoll for windows

This library implements the [epoll][man epoll] API for Windows
applications. It attempts to be efficient, and to match the Linux API
as semantics as closely as possible.

## Rationale

Unlike Linux, OS X, and many other operating systems, Windows doesn't
have a good API for receiving socket state notifications. It only
supports the `select` and `WSAPoll` APIs, but they
[don't scale][select scale] and suffer from
[other issues][wsapoll broken].

Using I/O completion ports isn't always practical when software is
designed to be cross-platform. Wepoll offers an alternative that is
much closer to a drop-in replacement for software that was designed
to run on Linux.

## Features

* can poll 100000s of sockets efficiently
* fully thread safe
* multiple threads can poll the same epoll port
* sockets can be added to multiple epoll sets
* polling for `EPOLLIN`, `EPOLLOUT`, `EPOLLPRI`, `EPOLLRDHUP`,
  `EPOLLHUP`, and `EPOLLERR` events
* `EPOLLONESTHOT` flag

## Limitations

* only works with sockets
* some modes not suported: `EPOLLET`, `EPOLLEXCLUSIVE`

## How to use

The library is [distributed][dist] as a single source file
([wepoll.c][wepoll.c]) and a single header file ([wepoll.h][wepoll.h]).
Compile the .c file as part of your project, and include the header
wherever needed.

## Compatibility

* Requires Windows 7 or higher.
* Can be compiled with recent versions of MSVC and Clang.
* GCC (mingw) should work (but is untested).

## API notes

### General

* The epoll port is a `HANDLE`, not a file descriptor.
* All functions set both `errno` and `GetLastError()` on failure.

### epoll_create/epoll_create1

```c
HANDLE epoll_create(int size);
HANDLE epoll_create1(int flags);
```

* Create a new epoll instance (port).
* `size` is ignored but most be greater than zero.
* `flags` must be zero as there are no supported flags.
* Returns `NULL` on failure.
* [man page][man epoll_create]

### epoll_close

```c
int epoll_close(HANDLE ephnd);
```

* Close an epoll port.
* Do not attempt to close the epoll port with `close()`,
  `CloseHandle()` or `closesocket()`.

### epoll_ctl

```c
int epoll_ctl(HANDLE ephnd,
              int op,
              SOCKET sock,
              struct epoll_event* event);
```

* Control which socket events are monitored by an epoll port.
* `ephnd` must be a HANDLE created by `epoll_create` or `epoll_create1`.
* `op` must be one of `EPOLL_CTL_ADD`, `EPOLL_CTL_MOD`, `EPOLL_CTL_DEL`.
* It is recommended to always explicitly remove a socket from its epoll
  set using `EPOLL_CTL_DEL` *before* closing it. As on Linux, sockets
  are automatically removed from the epoll set when they are closed, but
  wepoll may not be able to detect this until the next call to
  `epoll_wait()`.
* TODO: expand
* [man page][man epoll_ctl]

### epoll_wait

```c
int epoll_wait(HANDLE ephnd,
               struct epoll_event* events,
               int maxevents,
               int timeout);
```

* Receive socket events from an epoll port.
* Returns
  - -1 on failure
  -  0 when a timeout occurs
  - ≥1 the number of evens received
* TODO: expand
* [man page][man epoll_wait]

[dist]:             https://github.com/piscisaureus/wepoll/tree/dist
[man epoll]:        http://man7.org/linux/man-pages/man7/epoll.7.html
[man epoll_create]: http://man7.org/linux/man-pages/man2/epoll_create.2.html
[man epoll_ctl]:    http://man7.org/linux/man-pages/man2/epoll_ctl.2.html
[man epoll_wait]:   http://man7.org/linux/man-pages/man2/epoll_wait.2.html
[select scale]:     https://daniel.haxx.se/docs/poll-vs-select.html
[wsapoll broken]:   https://daniel.haxx.se/blog/2012/10/10/wsapoll-is-broken/
[wepoll.c]:         https://github.com/piscisaureus/wepoll/blob/dist/wepoll.c
[wepoll.h]:         https://github.com/piscisaureus/wepoll/blob/dist/wepoll.h
