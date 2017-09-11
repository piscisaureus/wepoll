#ifndef EPOLL_NT_H_
#define EPOLL_NT_H_

#include "internal.h"
#include "ntstatus.h"
#include "win.h"

EPOLL_INTERNAL int nt_init(void);

typedef struct _IO_STATUS_BLOCK {
  union {
    NTSTATUS Status;
    PVOID Pointer;
  };
  ULONG_PTR Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;

typedef VOID(NTAPI* PIO_APC_ROUTINE)(PVOID ApcContext,
                                     PIO_STATUS_BLOCK IoStatusBlock,
                                     ULONG Reserved);

#define NTDLL_IMPORT_LIST(X)         \
  X(NTSTATUS,                        \
    NTAPI,                           \
    NtDeviceIoControlFile,           \
    (HANDLE FileHandle,              \
     HANDLE Event,                   \
     PIO_APC_ROUTINE ApcRoutine,     \
     PVOID ApcContext,               \
     PIO_STATUS_BLOCK IoStatusBlock, \
     ULONG IoControlCode,            \
     PVOID InputBuffer,              \
     ULONG InputBufferLength,        \
     PVOID OutputBuffer,             \
     ULONG OutputBufferLength))      \
  X(ULONG, WINAPI, RtlNtStatusToDosError, (NTSTATUS Status))

#define X(return_type, declarators, name, parameters) \
  EPOLL_INTERNAL_EXTERN return_type(declarators* name) parameters;
NTDLL_IMPORT_LIST(X)
#undef X

#endif /* EPOLL_NT_H_ */
