#ifndef EPOLL_NT_H_
#define EPOLL_NT_H_

#include "ntstatus.h"
#include "win.h"

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

typedef NTSTATUS(NTAPI* PNTDEVICEIOCONTROLFILE)(HANDLE FileHandle,
                                                HANDLE Event,
                                                PIO_APC_ROUTINE ApcRoutine,
                                                PVOID ApcContext,
                                                PIO_STATUS_BLOCK IoStatusBlock,
                                                ULONG IoControlCode,
                                                PVOID InputBuffer,
                                                ULONG InputBufferLength,
                                                PVOID OutputBuffer,
                                                ULONG OutputBufferLength);

#endif /* EPOLL_NT_H_ */
