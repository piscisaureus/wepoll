#ifndef EPOLL_NT_H_
#define EPOLL_NT_H_

#include "win.h"

#ifndef _NTDEF_
typedef LONG NTSTATUS;
typedef NTSTATUS* PNTSTATUS;
#endif

#ifndef STATUS_SEVERITY_SUCCESS
#define STATUS_SEVERITY_SUCCESS 0x0
#endif

#ifndef STATUS_SEVERITY_INFORMATIONAL
#define STATUS_SEVERITY_INFORMATIONAL 0x1
#endif

#ifndef STATUS_SEVERITY_WARNING
#define STATUS_SEVERITY_WARNING 0x2
#endif

#ifndef STATUS_SEVERITY_ERROR
#define STATUS_SEVERITY_ERROR 0x3
#endif

#ifndef FACILITY_NTWIN32
#define FACILITY_NTWIN32 0x7
#endif

#ifndef NT_SUCCESS
#define NT_SUCCESS(status) (((NTSTATUS)(status)) >= 0)
#endif

#ifndef STATUS_SUCCESS
#define STATUS_SUCCESS ((NTSTATUS) 0x00000000L)
#endif

#ifndef STATUS_WAIT_0
#define STATUS_WAIT_0 ((NTSTATUS) 0x00000000L)
#endif

#ifndef STATUS_WAIT_1
#define STATUS_WAIT_1 ((NTSTATUS) 0x00000001L)
#endif

#ifndef STATUS_WAIT_2
#define STATUS_WAIT_2 ((NTSTATUS) 0x00000002L)
#endif

#ifndef STATUS_WAIT_3
#define STATUS_WAIT_3 ((NTSTATUS) 0x00000003L)
#endif

#ifndef STATUS_WAIT_63
#define STATUS_WAIT_63 ((NTSTATUS) 0x0000003FL)
#endif

#ifndef STATUS_ABANDONED
#define STATUS_ABANDONED ((NTSTATUS) 0x00000080L)
#endif

#ifndef STATUS_ABANDONED_WAIT_0
#define STATUS_ABANDONED_WAIT_0 ((NTSTATUS) 0x00000080L)
#endif

#ifndef STATUS_ABANDONED_WAIT_63
#define STATUS_ABANDONED_WAIT_63 ((NTSTATUS) 0x000000BFL)
#endif

#ifndef STATUS_USER_APC
#define STATUS_USER_APC ((NTSTATUS) 0x000000C0L)
#endif

#ifndef STATUS_KERNEL_APC
#define STATUS_KERNEL_APC ((NTSTATUS) 0x00000100L)
#endif

#ifndef STATUS_ALERTED
#define STATUS_ALERTED ((NTSTATUS) 0x00000101L)
#endif

#ifndef STATUS_TIMEOUT
#define STATUS_TIMEOUT ((NTSTATUS) 0x00000102L)
#endif

#ifndef STATUS_PENDING
#define STATUS_PENDING ((NTSTATUS) 0x00000103L)
#endif

/* This is not the NTSTATUS_FROM_WIN32 that the DDK provides, because the
 * DDK got it wrong!
 */
#ifdef NTSTATUS_FROM_WIN32
#undef NTSTATUS_FROM_WIN32
#endif
#define NTSTATUS_FROM_WIN32(error)                                      \
  ((NTSTATUS)(error) <= 0                                               \
       ? ((NTSTATUS)(error))                                            \
       : ((NTSTATUS)(((error) &0x0000FFFF) | (FACILITY_NTWIN32 << 16) | \
                     ERROR_SEVERITY_WARNING)))

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
