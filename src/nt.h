#ifndef EPOLL_NT_H_
#define EPOLL_NT_H_

#include "internal.h"
#include "ntstatus.h"
#include "win.h"

EPOLL_INTERNAL int nt_global_init(void);

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

typedef struct _LSA_UNICODE_STRING {
  USHORT Length;
  USHORT MaximumLength;
  PWSTR Buffer;
} LSA_UNICODE_STRING, *PLSA_UNICODE_STRING, UNICODE_STRING, *PUNICODE_STRING;

typedef struct _OBJECT_ATTRIBUTES {
  ULONG Length;
  HANDLE RootDirectory;
  PUNICODE_STRING ObjectName;
  ULONG Attributes;
  PVOID SecurityDescriptor;
  PVOID SecurityQualityOfService;
} OBJECT_ATTRIBUTES, *POBJECT_ATTRIBUTES;

#define NTDLL_IMPORT_LIST(X)                                                 \
  X(NTSTATUS,                                                                \
    NTAPI,                                                                   \
    NtDeviceIoControlFile,                                                   \
    (HANDLE FileHandle,                                                      \
     HANDLE Event,                                                           \
     PIO_APC_ROUTINE ApcRoutine,                                             \
     PVOID ApcContext,                                                       \
     PIO_STATUS_BLOCK IoStatusBlock,                                         \
     ULONG IoControlCode,                                                    \
     PVOID InputBuffer,                                                      \
     ULONG InputBufferLength,                                                \
     PVOID OutputBuffer,                                                     \
     ULONG OutputBufferLength))                                              \
                                                                             \
  X(ULONG, WINAPI, RtlNtStatusToDosError, (NTSTATUS Status))                 \
                                                                             \
  X(NTSTATUS,                                                                \
    NTAPI,                                                                   \
    NtCreateKeyedEvent,                                                      \
    (PHANDLE handle,                                                         \
     ACCESS_MASK access,                                                     \
     POBJECT_ATTRIBUTES attr,                                                \
     ULONG flags))                                                           \
                                                                             \
  X(NTSTATUS,                                                                \
    NTAPI,                                                                   \
    NtWaitForKeyedEvent,                                                     \
    (HANDLE handle, PVOID key, BOOLEAN alertable, PLARGE_INTEGER mstimeout)) \
                                                                             \
  X(NTSTATUS,                                                                \
    NTAPI,                                                                   \
    NtReleaseKeyedEvent,                                                     \
    (HANDLE handle, PVOID key, BOOLEAN alertable, PLARGE_INTEGER mstimeout))

#define X(return_type, declarators, name, parameters) \
  EPOLL_INTERNAL_EXTERN return_type(declarators* name) parameters;
NTDLL_IMPORT_LIST(X)
#undef X

#endif /* EPOLL_NT_H_ */
