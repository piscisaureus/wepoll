#ifndef WEPOLL_NT_H_
#define WEPOLL_NT_H_

#include "config.h"
#include "win.h"

WEPOLL_INTERNAL int nt_global_init(void);

typedef LONG NTSTATUS;
typedef NTSTATUS* PNTSTATUS;

#ifndef NT_SUCCESS
#define NT_SUCCESS(status) (((NTSTATUS)(status)) >= 0)
#endif

#ifndef STATUS_SUCCESS
#define STATUS_SUCCESS ((NTSTATUS) 0x00000000L)
#endif

#ifndef STATUS_PENDING
#define STATUS_PENDING ((NTSTATUS) 0x00000103L)
#endif

#ifndef STATUS_CANCELLED
#define STATUS_CANCELLED ((NTSTATUS) 0xC0000120L)
#endif

#ifndef STATUS_NOT_FOUND
#define STATUS_NOT_FOUND ((NTSTATUS) 0xC0000225L)
#endif

typedef struct _IO_STATUS_BLOCK {
  NTSTATUS Status;
  ULONG_PTR Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;

typedef VOID(NTAPI* PIO_APC_ROUTINE)(PVOID ApcContext,
                                     PIO_STATUS_BLOCK IoStatusBlock,
                                     ULONG Reserved);

typedef struct _UNICODE_STRING {
  USHORT Length;
  USHORT MaximumLength;
  PWSTR Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

#define RTL_CONSTANT_STRING(s) \
  { sizeof(s) - sizeof((s)[0]), sizeof(s), s }

typedef struct _OBJECT_ATTRIBUTES {
  ULONG Length;
  HANDLE RootDirectory;
  PUNICODE_STRING ObjectName;
  ULONG Attributes;
  PVOID SecurityDescriptor;
  PVOID SecurityQualityOfService;
} OBJECT_ATTRIBUTES, *POBJECT_ATTRIBUTES;

#define RTL_CONSTANT_OBJECT_ATTRIBUTES(ObjectName, Attributes) \
  { sizeof(OBJECT_ATTRIBUTES), NULL, ObjectName, Attributes, NULL, NULL }

#ifndef FILE_OPEN
#define FILE_OPEN 0x00000001UL
#endif

#define NT_NTDLL_IMPORT_LIST(X)           \
  X(NTSTATUS,                             \
    NTAPI,                                \
    NtCancelIoFileEx,                     \
    (HANDLE FileHandle,                   \
     PIO_STATUS_BLOCK IoRequestToCancel,  \
     PIO_STATUS_BLOCK IoStatusBlock))     \
                                          \
  X(NTSTATUS,                             \
    NTAPI,                                \
    NtCreateFile,                         \
    (PHANDLE FileHandle,                  \
     ACCESS_MASK DesiredAccess,           \
     POBJECT_ATTRIBUTES ObjectAttributes, \
     PIO_STATUS_BLOCK IoStatusBlock,      \
     PLARGE_INTEGER AllocationSize,       \
     ULONG FileAttributes,                \
     ULONG ShareAccess,                   \
     ULONG CreateDisposition,             \
     ULONG CreateOptions,                 \
     PVOID EaBuffer,                      \
     ULONG EaLength))                     \
                                          \
  X(NTSTATUS,                             \
    NTAPI,                                \
    NtDeviceIoControlFile,                \
    (HANDLE FileHandle,                   \
     HANDLE Event,                        \
     PIO_APC_ROUTINE ApcRoutine,          \
     PVOID ApcContext,                    \
     PIO_STATUS_BLOCK IoStatusBlock,      \
     ULONG IoControlCode,                 \
     PVOID InputBuffer,                   \
     ULONG InputBufferLength,             \
     PVOID OutputBuffer,                  \
     ULONG OutputBufferLength))           \
                                          \
  X(ULONG, WINAPI, RtlNtStatusToDosError, (NTSTATUS Status))

#define X(return_type, attributes, name, parameters) \
  WEPOLL_INTERNAL_EXTERN return_type(attributes* name) parameters;
NT_NTDLL_IMPORT_LIST(X)
#undef X

#endif /* WEPOLL_NT_H_ */
