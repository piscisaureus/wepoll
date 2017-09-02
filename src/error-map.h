#ifndef ERROR_MAP_H_
#define ERROR_MAP_H_

/* clang-format off */

#define WE_ERROR_MAP(X)                                         \
  X(0x00000000, STATUS_SUCCESS,                                 \
    ERROR_SUCCESS,                                              \
    ERROR_SUCCESS, 0)                                           \
  X(0x00000001, STATUS_WAIT_1,                                  \
    ERROR_WAIT_1,                                               \
    -1, -1)                                                     \
  X(0x00000002, STATUS_WAIT_2,                                  \
    ERROR_WAIT_2,                                               \
    -1, -1)                                                     \
  X(0x00000003, STATUS_WAIT_3,                                  \
    ERROR_WAIT_3,                                               \
    -1, -1)                                                     \
  X(0x0000003f, STATUS_WAIT_63,                                 \
    ERROR_WAIT_63,                                              \
    -1, -1)                                                     \
  X(0x00000080, STATUS_ABANDONED,                               \
    ERROR_WAIT_NO_CHILDREN,                                     \
    -1, ECHILD)                                                 \
  X(0x000000bf, STATUS_ABANDONED_WAIT_63,                       \
    ERROR_ABANDONED_WAIT_63,                                    \
    -1, -1)                                                     \
  X(0x000000c0, STATUS_USER_APC,                                \
    ERROR_USER_APC,                                             \
    -1, -1)                                                     \
  X(0x000000ff, /* ntstatus symbol not available */,            \
    ERROR_SUCCESS,                                              \
    ERROR_SUCCESS, 0)                                           \
  X(0x00000100, STATUS_KERNEL_APC,                              \
    ERROR_KERNEL_APC,                                           \
    -1, -1)                                                     \
  X(0x00000101, STATUS_ALERTED,                                 \
    ERROR_ALERTED,                                              \
    -1, -1)                                                     \
  X(0x00000102, STATUS_TIMEOUT,                                 \
    ERROR_TIMEOUT,                                              \
    WSAETIMEDOUT, ETIMEDOUT)                                    \
  X(0x00000103, STATUS_PENDING,                                 \
    ERROR_IO_PENDING,                                           \
    ERROR_IO_PENDING, -1)                                       \
  X(0x00000104, STATUS_REPARSE,                                 \
    ERROR_REPARSE,                                              \
    -1, -1)                                                     \
  X(0x00000105, STATUS_MORE_ENTRIES,                            \
    ERROR_MORE_DATA,                                            \
    -1, -1)                                                     \
  X(0x00000106, STATUS_NOT_ALL_ASSIGNED,                        \
    ERROR_NOT_ALL_ASSIGNED,                                     \
    -1, -1)                                                     \
  X(0x00000107, STATUS_SOME_NOT_MAPPED,                         \
    ERROR_SOME_NOT_MAPPED,                                      \
    -1, -1)                                                     \
  X(0x00000108, STATUS_OPLOCK_BREAK_IN_PROGRESS,                \
    ERROR_OPLOCK_BREAK_IN_PROGRESS,                             \
    -1, -1)                                                     \
  X(0x00000109, STATUS_VOLUME_MOUNTED,                          \
    ERROR_VOLUME_MOUNTED,                                       \
    -1, -1)                                                     \
  X(0x0000010a, STATUS_RXACT_COMMITTED,                         \
    ERROR_RXACT_COMMITTED,                                      \
    -1, -1)                                                     \
  X(0x0000010b, STATUS_NOTIFY_CLEANUP,                          \
    ERROR_NOTIFY_CLEANUP,                                       \
    -1, -1)                                                     \
  X(0x0000010c, STATUS_NOTIFY_ENUM_DIR,                         \
    ERROR_NOTIFY_ENUM_DIR,                                      \
    -1, -1)                                                     \
  X(0x0000010d, STATUS_NO_QUOTAS_FOR_ACCOUNT,                   \
    ERROR_NO_QUOTAS_FOR_ACCOUNT,                                \
    -1, -1)                                                     \
  X(0x0000010e, STATUS_PRIMARY_TRANSPORT_CONNECT_FAILED,        \
    ERROR_PRIMARY_TRANSPORT_CONNECT_FAILED,                     \
    -1, -1)                                                     \
  X(0x00000110, STATUS_PAGE_FAULT_TRANSITION,                   \
    ERROR_PAGE_FAULT_TRANSITION,                                \
    -1, -1)                                                     \
  X(0x00000111, STATUS_PAGE_FAULT_DEMAND_ZERO,                  \
    ERROR_PAGE_FAULT_DEMAND_ZERO,                               \
    -1, -1)                                                     \
  X(0x00000112, STATUS_PAGE_FAULT_COPY_ON_WRITE,                \
    ERROR_PAGE_FAULT_COPY_ON_WRITE,                             \
    -1, -1)                                                     \
  X(0x00000113, STATUS_PAGE_FAULT_GUARD_PAGE,                   \
    ERROR_PAGE_FAULT_GUARD_PAGE,                                \
    -1, -1)                                                     \
  X(0x00000114, STATUS_PAGE_FAULT_PAGING_FILE,                  \
    ERROR_PAGE_FAULT_PAGING_FILE,                               \
    -1, -1)                                                     \
  X(0x00000115, STATUS_CACHE_PAGE_LOCKED,                       \
    ERROR_CACHE_PAGE_LOCKED,                                    \
    -1, -1)                                                     \
  X(0x00000116, STATUS_CRASH_DUMP,                              \
    ERROR_CRASH_DUMP,                                           \
    -1, -1)                                                     \
  X(0x00000117, STATUS_BUFFER_ALL_ZEROS,                        \
    ERROR_BUFFER_ALL_ZEROS,                                     \
    -1, -1)                                                     \
  X(0x00000118, STATUS_REPARSE_OBJECT,                          \
    ERROR_REPARSE_OBJECT,                                       \
    -1, -1)                                                     \
  X(0x00000119, STATUS_RESOURCE_REQUIREMENTS_CHANGED,           \
    ERROR_RESOURCE_REQUIREMENTS_CHANGED,                        \
    -1, -1)                                                     \
  X(0x00000120, STATUS_TRANSLATION_COMPLETE,                    \
    ERROR_TRANSLATION_COMPLETE,                                 \
    -1, -1)                                                     \
  X(0x00000121, STATUS_DS_MEMBERSHIP_EVALUATED_LOCALLY,         \
    ERROR_DS_MEMBERSHIP_EVALUATED_LOCALLY,                      \
    -1, -1)                                                     \
  X(0x00000122, STATUS_NOTHING_TO_TERMINATE,                    \
    ERROR_NOTHING_TO_TERMINATE,                                 \
    -1, -1)                                                     \
  X(0x00000123, STATUS_PROCESS_NOT_IN_JOB,                      \
    ERROR_PROCESS_NOT_IN_JOB,                                   \
    -1, -1)                                                     \
  X(0x00000124, STATUS_PROCESS_IN_JOB,                          \
    ERROR_PROCESS_IN_JOB,                                       \
    -1, -1)                                                     \
  X(0x00000125, STATUS_VOLSNAP_HIBERNATE_READY,                 \
    ERROR_VOLSNAP_HIBERNATE_READY,                              \
    -1, -1)                                                     \
  X(0x00000126, STATUS_FSFILTER_OP_COMPLETED_SUCCESSFULLY,      \
    ERROR_FSFILTER_OP_COMPLETED_SUCCESSFULLY,                   \
    -1, -1)                                                     \
  X(0x00000127, STATUS_INTERRUPT_VECTOR_ALREADY_CONNECTED,      \
    ERROR_INTERRUPT_VECTOR_ALREADY_CONNECTED,                   \
    -1, -1)                                                     \
  X(0x00000128, STATUS_INTERRUPT_STILL_CONNECTED,               \
    ERROR_INTERRUPT_STILL_CONNECTED,                            \
    -1, -1)                                                     \
  X(0x00000129, STATUS_PROCESS_CLONED,                          \
    ERROR_SUCCESS,                                              \
    ERROR_SUCCESS, 0)                                           \
  X(0x0000012a, STATUS_FILE_LOCKED_WITH_ONLY_READERS,           \
    ERROR_SUCCESS,                                              \
    ERROR_SUCCESS, 0)                                           \
  X(0x0000012b, STATUS_FILE_LOCKED_WITH_WRITERS,                \
    ERROR_SUCCESS,                                              \
    ERROR_SUCCESS, 0)                                           \
  X(0x0000012c, /* ntstatus symbol not available */,            \
    ERROR_SUCCESS,                                              \
    ERROR_SUCCESS, 0)                                           \
  X(0x0000012d, /* ntstatus symbol not available */,            \
    ERROR_SUCCESS,                                              \
    ERROR_SUCCESS, 0)                                           \
  X(0x0000012e, /* ntstatus symbol not available */,            \
    ERROR_SUCCESS,                                              \
    ERROR_SUCCESS, 0)                                           \
  X(0x0000012f, /* ntstatus symbol not available */,            \
    ERROR_SUCCESS,                                              \
    ERROR_SUCCESS, 0)                                           \
  X(0x00000202, STATUS_RESOURCEMANAGER_READ_ONLY,               \
    ERROR_RESOURCEMANAGER_READ_ONLY,                            \
    -1, -1)                                                     \
  X(0x00000210, STATUS_RING_PREVIOUSLY_EMPTY,                   \
    ERROR_SUCCESS,                                              \
    ERROR_SUCCESS, 0)                                           \
  X(0x00000211, STATUS_RING_PREVIOUSLY_FULL,                    \
    ERROR_SUCCESS,                                              \
    ERROR_SUCCESS, 0)                                           \
  X(0x00000212, STATUS_RING_PREVIOUSLY_ABOVE_QUOTA,             \
    ERROR_SUCCESS,                                              \
    ERROR_SUCCESS, 0)                                           \
  X(0x00000213, STATUS_RING_NEWLY_EMPTY,                        \
    ERROR_SUCCESS,                                              \
    ERROR_SUCCESS, 0)                                           \
  X(0x00000214, STATUS_RING_SIGNAL_OPPOSITE_ENDPOINT,           \
    ERROR_SUCCESS,                                              \
    ERROR_SUCCESS, 0)                                           \
  X(0x00000215, STATUS_OPLOCK_SWITCHED_TO_NEW_HANDLE,           \
    ERROR_OPLOCK_SWITCHED_TO_NEW_HANDLE,                        \
    -1, -1)                                                     \
  X(0x00000216, STATUS_OPLOCK_HANDLE_CLOSED,                    \
    ERROR_OPLOCK_HANDLE_CLOSED,                                 \
    -1, -1)                                                     \
  X(0x00000367, STATUS_WAIT_FOR_OPLOCK,                         \
    ERROR_WAIT_FOR_OPLOCK,                                      \
    -1, -1)                                                     \
  X(0x00000368, /* ntstatus symbol not available */,            \
    ERROR_REPARSE,                                              \
    -1, -1)                                                     \
  X(0x40000000, STATUS_OBJECT_NAME_EXISTS,                      \
    ERROR_OBJECT_NAME_EXISTS,                                   \
    -1, -1)                                                     \
  X(0x40000001, STATUS_THREAD_WAS_SUSPENDED,                    \
    ERROR_THREAD_WAS_SUSPENDED,                                 \
    -1, -1)                                                     \
  X(0x40000002, STATUS_WORKING_SET_LIMIT_RANGE,                 \
    ERROR_INVALID_PARAMETER,                                    \
    -1, EINVAL)                                                 \
  X(0x40000003, STATUS_IMAGE_NOT_AT_BASE,                       \
    ERROR_IMAGE_NOT_AT_BASE,                                    \
    -1, -1)                                                     \
  X(0x40000004, STATUS_RXACT_STATE_CREATED,                     \
    ERROR_RXACT_STATE_CREATED,                                  \
    -1, -1)                                                     \
  X(0x40000005, STATUS_SEGMENT_NOTIFICATION,                    \
    ERROR_SEGMENT_NOTIFICATION,                                 \
    -1, -1)                                                     \
  X(0x40000006, STATUS_LOCAL_USER_SESSION_KEY,                  \
    ERROR_LOCAL_USER_SESSION_KEY,                               \
    -1, -1)                                                     \
  X(0x40000007, STATUS_BAD_CURRENT_DIRECTORY,                   \
    ERROR_BAD_CURRENT_DIRECTORY,                                \
    -1, -1)                                                     \
  X(0x40000008, STATUS_SERIAL_MORE_WRITES,                      \
    ERROR_MORE_WRITES,                                          \
    -1, -1)                                                     \
  X(0x40000009, STATUS_REGISTRY_RECOVERED,                      \
    ERROR_REGISTRY_RECOVERED,                                   \
    -1, -1)                                                     \
  X(0x4000000a, STATUS_FT_READ_RECOVERY_FROM_BACKUP,            \
    ERROR_FT_READ_RECOVERY_FROM_BACKUP,                         \
    -1, -1)                                                     \
  X(0x4000000b, STATUS_FT_WRITE_RECOVERY,                       \
    ERROR_FT_WRITE_RECOVERY,                                    \
    -1, -1)                                                     \
  X(0x4000000c, STATUS_SERIAL_COUNTER_TIMEOUT,                  \
    ERROR_COUNTER_TIMEOUT,                                      \
    -1, -1)                                                     \
  X(0x4000000d, STATUS_NULL_LM_PASSWORD,                        \
    ERROR_NULL_LM_PASSWORD,                                     \
    -1, -1)                                                     \
  X(0x4000000e, STATUS_IMAGE_MACHINE_TYPE_MISMATCH,             \
    ERROR_IMAGE_MACHINE_TYPE_MISMATCH,                          \
    -1, -1)                                                     \
  X(0x4000000f, STATUS_RECEIVE_PARTIAL,                         \
    ERROR_RECEIVE_PARTIAL,                                      \
    -1, -1)                                                     \
  X(0x40000010, STATUS_RECEIVE_EXPEDITED,                       \
    ERROR_RECEIVE_EXPEDITED,                                    \
    -1, -1)                                                     \
  X(0x40000011, STATUS_RECEIVE_PARTIAL_EXPEDITED,               \
    ERROR_RECEIVE_PARTIAL_EXPEDITED,                            \
    -1, -1)                                                     \
  X(0x40000012, STATUS_EVENT_DONE,                              \
    ERROR_EVENT_DONE,                                           \
    -1, -1)                                                     \
  X(0x40000013, STATUS_EVENT_PENDING,                           \
    ERROR_EVENT_PENDING,                                        \
    -1, -1)                                                     \
  X(0x40000014, STATUS_CHECKING_FILE_SYSTEM,                    \
    ERROR_CHECKING_FILE_SYSTEM,                                 \
    -1, -1)                                                     \
  X(0x40000015, STATUS_FATAL_APP_EXIT,                          \
    ERROR_FATAL_APP_EXIT,                                       \
    -1, -1)                                                     \
  X(0x40000016, STATUS_PREDEFINED_HANDLE,                       \
    ERROR_PREDEFINED_HANDLE,                                    \
    -1, -1)                                                     \
  X(0x40000017, STATUS_WAS_UNLOCKED,                            \
    ERROR_WAS_UNLOCKED,                                         \
    -1, -1)                                                     \
  X(0x40000018, STATUS_SERVICE_NOTIFICATION,                    \
    ERROR_SERVICE_NOTIFICATION,                                 \
    -1, -1)                                                     \
  X(0x40000019, STATUS_WAS_LOCKED,                              \
    ERROR_WAS_LOCKED,                                           \
    -1, -1)                                                     \
  X(0x4000001a, STATUS_LOG_HARD_ERROR,                          \
    ERROR_LOG_HARD_ERROR,                                       \
    -1, -1)                                                     \
  X(0x4000001b, STATUS_ALREADY_WIN32,                           \
    ERROR_ALREADY_WIN32,                                        \
    -1, -1)                                                     \
  X(0x4000001c, STATUS_WX86_UNSIMULATE,                         \
    ERROR_WX86_WARNING,                                         \
    -1, -1)                                                     \
  X(0x4000001d, STATUS_WX86_CONTINUE,                           \
    ERROR_WX86_WARNING,                                         \
    -1, -1)                                                     \
  X(0x4000001e, STATUS_WX86_SINGLE_STEP,                        \
    ERROR_WX86_WARNING,                                         \
    -1, -1)                                                     \
  X(0x4000001f, STATUS_WX86_BREAKPOINT,                         \
    ERROR_WX86_WARNING,                                         \
    -1, -1)                                                     \
  X(0x40000020, STATUS_WX86_EXCEPTION_CONTINUE,                 \
    ERROR_WX86_WARNING,                                         \
    -1, -1)                                                     \
  X(0x40000021, STATUS_WX86_EXCEPTION_LASTCHANCE,               \
    ERROR_WX86_WARNING,                                         \
    -1, -1)                                                     \
  X(0x40000022, STATUS_WX86_EXCEPTION_CHAIN,                    \
    ERROR_WX86_WARNING,                                         \
    -1, -1)                                                     \
  X(0x40000023, STATUS_IMAGE_MACHINE_TYPE_MISMATCH_EXE,         \
    ERROR_IMAGE_MACHINE_TYPE_MISMATCH_EXE,                      \
    -1, -1)                                                     \
  X(0x40000024, STATUS_NO_YIELD_PERFORMED,                      \
    ERROR_NO_YIELD_PERFORMED,                                   \
    -1, -1)                                                     \
  X(0x40000025, STATUS_TIMER_RESUME_IGNORED,                    \
    ERROR_TIMER_RESUME_IGNORED,                                 \
    -1, -1)                                                     \
  X(0x40000026, STATUS_ARBITRATION_UNHANDLED,                   \
    ERROR_ARBITRATION_UNHANDLED,                                \
    -1, -1)                                                     \
  X(0x40000027, STATUS_CARDBUS_NOT_SUPPORTED,                   \
    ERROR_CARDBUS_NOT_SUPPORTED,                                \
    -1, -1)                                                     \
  X(0x40000028, STATUS_WX86_CREATEWX86TIB,                      \
    ERROR_WX86_WARNING,                                         \
    -1, -1)                                                     \
  X(0x40000029, STATUS_MP_PROCESSOR_MISMATCH,                   \
    ERROR_MP_PROCESSOR_MISMATCH,                                \
    -1, -1)                                                     \
  X(0x4000002a, STATUS_HIBERNATED,                              \
    ERROR_HIBERNATED,                                           \
    -1, -1)                                                     \
  X(0x4000002b, STATUS_RESUME_HIBERNATION,                      \
    ERROR_RESUME_HIBERNATION,                                   \
    -1, -1)                                                     \
  X(0x4000002c, STATUS_FIRMWARE_UPDATED,                        \
    ERROR_FIRMWARE_UPDATED,                                     \
    -1, -1)                                                     \
  X(0x4000002d, STATUS_DRIVERS_LEAKING_LOCKED_PAGES,            \
    ERROR_DRIVERS_LEAKING_LOCKED_PAGES,                         \
    -1, -1)                                                     \
  X(0x4000002e, STATUS_MESSAGE_RETRIEVED,                       \
    ERROR_INTERNAL_ERROR,                                       \
    -1, -1)                                                     \
  X(0x4000002f, STATUS_SYSTEM_POWERSTATE_TRANSITION,            \
    ERROR_SYSTEM_POWERSTATE_TRANSITION,                         \
    -1, -1)                                                     \
  X(0x40000030, STATUS_ALPC_CHECK_COMPLETION_LIST,              \
    ERROR_INTERNAL_ERROR,                                       \
    -1, -1)                                                     \
  X(0x40000031, STATUS_SYSTEM_POWERSTATE_COMPLEX_TRANSITION,    \
    ERROR_SYSTEM_POWERSTATE_COMPLEX_TRANSITION,                 \
    -1, -1)                                                     \
  X(0x40000032, STATUS_ACCESS_AUDIT_BY_POLICY,                  \
    ERROR_ACCESS_AUDIT_BY_POLICY,                               \
    -1, -1)                                                     \
  X(0x40000033, STATUS_ABANDON_HIBERFILE,                       \
    ERROR_ABANDON_HIBERFILE,                                    \
    -1, -1)                                                     \
  X(0x40000035, /* ntstatus symbol not available */,            \
    ERROR_SUCCESS,                                              \
    ERROR_SUCCESS, 0)                                           \
  X(0x40000294, STATUS_WAKE_SYSTEM,                             \
    ERROR_WAKE_SYSTEM,                                          \
    -1, -1)                                                     \
  X(0x40000370, STATUS_DS_SHUTTING_DOWN,                        \
    ERROR_DS_SHUTTING_DOWN,                                     \
    -1, -1)                                                     \
  X(0x80000002, STATUS_DATATYPE_MISALIGNMENT,                   \
    ERROR_NOACCESS,                                             \
    -1, -1)                                                     \
  X(0x80000005, STATUS_BUFFER_OVERFLOW,                         \
    ERROR_MORE_DATA,                                            \
    WSAEMSGSIZE, EMSGSIZE)                                      \
  X(0x80000006, STATUS_NO_MORE_FILES,                           \
    ERROR_NO_MORE_FILES,                                        \
    -1, ENOENT)                                                 \
  X(0x80000007, STATUS_WAKE_SYSTEM_DEBUGGER,                    \
    ERROR_WAKE_SYSTEM_DEBUGGER,                                 \
    -1, -1)                                                     \
  X(0x8000000a, STATUS_HANDLES_CLOSED,                          \
    ERROR_HANDLES_CLOSED,                                       \
    -1, -1)                                                     \
  X(0x8000000b, STATUS_NO_INHERITANCE,                          \
    ERROR_NO_INHERITANCE,                                       \
    -1, -1)                                                     \
  X(0x8000000c, STATUS_GUID_SUBSTITUTION_MADE,                  \
    ERROR_GUID_SUBSTITUTION_MADE,                               \
    -1, -1)                                                     \
  X(0x8000000d, STATUS_PARTIAL_COPY,                            \
    ERROR_PARTIAL_COPY,                                         \
    -1, -1)                                                     \
  X(0x8000000e, STATUS_DEVICE_PAPER_EMPTY,                      \
    ERROR_OUT_OF_PAPER,                                         \
    -1, EACCES)                                                 \
  X(0x8000000f, STATUS_DEVICE_POWERED_OFF,                      \
    ERROR_NOT_READY,                                            \
    -1, EACCES)                                                 \
  X(0x80000010, STATUS_DEVICE_OFF_LINE,                         \
    ERROR_NOT_READY,                                            \
    -1, EACCES)                                                 \
  X(0x80000011, STATUS_DEVICE_BUSY,                             \
    ERROR_BUSY,                                                 \
    -1, -1)                                                     \
  X(0x80000012, STATUS_NO_MORE_EAS,                             \
    ERROR_NO_MORE_ITEMS,                                        \
    -1, -1)                                                     \
  X(0x80000013, STATUS_INVALID_EA_NAME,                         \
    ERROR_INVALID_EA_NAME,                                      \
    -1, -1)                                                     \
  X(0x80000014, STATUS_EA_LIST_INCONSISTENT,                    \
    ERROR_EA_LIST_INCONSISTENT,                                 \
    -1, -1)                                                     \
  X(0x80000015, STATUS_INVALID_EA_FLAG,                         \
    ERROR_EA_LIST_INCONSISTENT,                                 \
    -1, -1)                                                     \
  X(0x80000016, STATUS_VERIFY_REQUIRED,                         \
    ERROR_MEDIA_CHANGED,                                        \
    -1, -1)                                                     \
  X(0x80000017, STATUS_EXTRANEOUS_INFORMATION,                  \
    ERROR_EXTRANEOUS_INFORMATION,                               \
    -1, -1)                                                     \
  X(0x80000018, STATUS_RXACT_COMMIT_NECESSARY,                  \
    ERROR_RXACT_COMMIT_NECESSARY,                               \
    -1, -1)                                                     \
  X(0x8000001a, STATUS_NO_MORE_ENTRIES,                         \
    ERROR_NO_MORE_ITEMS,                                        \
    -1, -1)                                                     \
  X(0x8000001b, STATUS_FILEMARK_DETECTED,                       \
    ERROR_FILEMARK_DETECTED,                                    \
    -1, -1)                                                     \
  X(0x8000001c, STATUS_MEDIA_CHANGED,                           \
    ERROR_MEDIA_CHANGED,                                        \
    -1, -1)                                                     \
  X(0x8000001d, STATUS_BUS_RESET,                               \
    ERROR_BUS_RESET,                                            \
    -1, -1)                                                     \
  X(0x8000001e, STATUS_END_OF_MEDIA,                            \
    ERROR_END_OF_MEDIA,                                         \
    -1, -1)                                                     \
  X(0x8000001f, STATUS_BEGINNING_OF_MEDIA,                      \
    ERROR_BEGINNING_OF_MEDIA,                                   \
    -1, -1)                                                     \
  X(0x80000020, STATUS_MEDIA_CHECK,                             \
    ERROR_MEDIA_CHECK,                                          \
    -1, -1)                                                     \
  X(0x80000021, STATUS_SETMARK_DETECTED,                        \
    ERROR_SETMARK_DETECTED,                                     \
    -1, -1)                                                     \
  X(0x80000022, STATUS_NO_DATA_DETECTED,                        \
    ERROR_NO_DATA_DETECTED,                                     \
    -1, -1)                                                     \
  X(0x80000023, STATUS_REDIRECTOR_HAS_OPEN_HANDLES,             \
    ERROR_REDIRECTOR_HAS_OPEN_HANDLES,                          \
    -1, -1)                                                     \
  X(0x80000024, STATUS_SERVER_HAS_OPEN_HANDLES,                 \
    ERROR_SERVER_HAS_OPEN_HANDLES,                              \
    -1, -1)                                                     \
  X(0x80000025, STATUS_ALREADY_DISCONNECTED,                    \
    ERROR_ACTIVE_CONNECTIONS,                                   \
    -1, -1)                                                     \
  X(0x80000026, STATUS_LONGJUMP,                                \
    ERROR_LONGJUMP,                                             \
    -1, -1)                                                     \
  X(0x80000027, STATUS_CLEANER_CARTRIDGE_INSTALLED,             \
    ERROR_CLEANER_CARTRIDGE_INSTALLED,                          \
    -1, -1)                                                     \
  X(0x80000028, STATUS_PLUGPLAY_QUERY_VETOED,                   \
    ERROR_PLUGPLAY_QUERY_VETOED,                                \
    -1, -1)                                                     \
  X(0x80000029, STATUS_UNWIND_CONSOLIDATE,                      \
    ERROR_UNWIND_CONSOLIDATE,                                   \
    -1, -1)                                                     \
  X(0x8000002a, STATUS_REGISTRY_HIVE_RECOVERED,                 \
    ERROR_REGISTRY_HIVE_RECOVERED,                              \
    -1, -1)                                                     \
  X(0x8000002b, STATUS_DLL_MIGHT_BE_INSECURE,                   \
    ERROR_DLL_MIGHT_BE_INSECURE,                                \
    -1, -1)                                                     \
  X(0x8000002c, STATUS_DLL_MIGHT_BE_INCOMPATIBLE,               \
    ERROR_DLL_MIGHT_BE_INCOMPATIBLE,                            \
    -1, -1)                                                     \
  X(0x8000002d, STATUS_STOPPED_ON_SYMLINK,                      \
    ERROR_STOPPED_ON_SYMLINK,                                   \
    -1, -1)                                                     \
  X(0x8000002e, STATUS_CANNOT_GRANT_REQUESTED_OPLOCK,           \
    ERROR_CANNOT_GRANT_REQUESTED_OPLOCK,                        \
    -1, -1)                                                     \
  X(0x8000002f, STATUS_NO_ACE_CONDITION,                        \
    ERROR_NO_ACE_CONDITION,                                     \
    -1, -1)                                                     \
  X(0x80000288, STATUS_DEVICE_REQUIRES_CLEANING,                \
    ERROR_DEVICE_REQUIRES_CLEANING,                             \
    -1, -1)                                                     \
  X(0x80000289, STATUS_DEVICE_DOOR_OPEN,                        \
    ERROR_DEVICE_DOOR_OPEN,                                     \
    -1, -1)                                                     \
  X(0x80000803, STATUS_DATA_LOST_REPAIR,                        \
    ERROR_DATA_LOST_REPAIR,                                     \
    -1, -1)                                                     \
  X(0xc0000001, STATUS_UNSUCCESSFUL,                            \
    ERROR_GEN_FAILURE,                                          \
    -1, EACCES)                                                 \
  X(0xc0000002, STATUS_NOT_IMPLEMENTED,                         \
    ERROR_INVALID_FUNCTION,                                     \
    WSAEOPNOTSUPP, EOPNOTSUPP)                                  \
  X(0xc0000003, STATUS_INVALID_INFO_CLASS,                      \
    ERROR_INVALID_PARAMETER,                                    \
    -1, EINVAL)                                                 \
  X(0xc0000004, STATUS_INFO_LENGTH_MISMATCH,                    \
    ERROR_BAD_LENGTH,                                           \
    -1, EACCES)                                                 \
  X(0xc0000005, STATUS_ACCESS_VIOLATION,                        \
    ERROR_NOACCESS,                                             \
    WSAEFAULT, EFAULT)                                          \
  X(0xc0000006, STATUS_IN_PAGE_ERROR,                           \
    ERROR_SWAPERROR,                                            \
    -1, -1)                                                     \
  X(0xc0000007, STATUS_PAGEFILE_QUOTA,                          \
    ERROR_PAGEFILE_QUOTA,                                       \
    WSAENOBUFS, ENOBUFS)                                        \
  X(0xc0000008, STATUS_INVALID_HANDLE,                          \
    ERROR_INVALID_HANDLE,                                       \
    WSAENOTSOCK, ENOTSOCK)                                      \
  X(0xc0000009, STATUS_BAD_INITIAL_STACK,                       \
    ERROR_STACK_OVERFLOW,                                       \
    -1, -1)                                                     \
  X(0xc000000a, STATUS_BAD_INITIAL_PC,                          \
    ERROR_BAD_EXE_FORMAT,                                       \
    -1, ENOEXEC)                                                \
  X(0xc000000b, STATUS_INVALID_CID,                             \
    ERROR_INVALID_PARAMETER,                                    \
    -1, EINVAL)                                                 \
  X(0xc000000c, STATUS_TIMER_NOT_CANCELED,                      \
    ERROR_TIMER_NOT_CANCELED,                                   \
    -1, -1)                                                     \
  X(0xc000000d, STATUS_INVALID_PARAMETER,                       \
    ERROR_INVALID_PARAMETER,                                    \
    -1, EINVAL)                                                 \
  X(0xc000000e, STATUS_NO_SUCH_DEVICE,                          \
    ERROR_FILE_NOT_FOUND,                                       \
    WSAENETDOWN, ENETDOWN)                                      \
  X(0xc000000f, STATUS_NO_SUCH_FILE,                            \
    ERROR_FILE_NOT_FOUND,                                       \
    WSAENETDOWN, ENETDOWN)                                      \
  X(0xc0000010, STATUS_INVALID_DEVICE_REQUEST,                  \
    ERROR_INVALID_FUNCTION,                                     \
    -1, EINVAL)                                                 \
  X(0xc0000011, STATUS_END_OF_FILE,                             \
    ERROR_HANDLE_EOF,                                           \
    -1, -1)                                                     \
  X(0xc0000012, STATUS_WRONG_VOLUME,                            \
    ERROR_WRONG_DISK,                                           \
    -1, EACCES)                                                 \
  X(0xc0000013, STATUS_NO_MEDIA_IN_DEVICE,                      \
    ERROR_NOT_READY,                                            \
    -1, EACCES)                                                 \
  X(0xc0000014, STATUS_UNRECOGNIZED_MEDIA,                      \
    ERROR_UNRECOGNIZED_MEDIA,                                   \
    -1, -1)                                                     \
  X(0xc0000015, STATUS_NONEXISTENT_SECTOR,                      \
    ERROR_SECTOR_NOT_FOUND,                                     \
    -1, EACCES)                                                 \
  X(0xc0000016, STATUS_MORE_PROCESSING_REQUIRED,                \
    ERROR_MORE_DATA,                                            \
    -1, -1)                                                     \
  X(0xc0000017, STATUS_NO_MEMORY,                               \
    ERROR_NOT_ENOUGH_MEMORY,                                    \
    WSAENOBUFS, ENOBUFS)                                        \
  X(0xc0000018, STATUS_CONFLICTING_ADDRESSES,                   \
    ERROR_INVALID_ADDRESS,                                      \
    WSAEADDRNOTAVAIL, EADDRNOTAVAIL)                            \
  X(0xc0000019, STATUS_NOT_MAPPED_VIEW,                         \
    ERROR_INVALID_ADDRESS,                                      \
    -1, -1)                                                     \
  X(0xc000001a, STATUS_UNABLE_TO_FREE_VM,                       \
    ERROR_INVALID_PARAMETER,                                    \
    -1, EINVAL)                                                 \
  X(0xc000001b, STATUS_UNABLE_TO_DELETE_SECTION,                \
    ERROR_INVALID_PARAMETER,                                    \
    -1, EINVAL)                                                 \
  X(0xc000001c, STATUS_INVALID_SYSTEM_SERVICE,                  \
    ERROR_INVALID_FUNCTION,                                     \
    -1, EINVAL)                                                 \
  X(0xc000001e, STATUS_INVALID_LOCK_SEQUENCE,                   \
    ERROR_ACCESS_DENIED,                                        \
    -1, EACCES)                                                 \
  X(0xc000001f, STATUS_INVALID_VIEW_SIZE,                       \
    ERROR_ACCESS_DENIED,                                        \
    -1, EACCES)                                                 \
  X(0xc0000020, STATUS_INVALID_FILE_FOR_SECTION,                \
    ERROR_BAD_EXE_FORMAT,                                       \
    -1, ENOEXEC)                                                \
  X(0xc0000021, STATUS_ALREADY_COMMITTED,                       \
    ERROR_ACCESS_DENIED,                                        \
    -1, EACCES)                                                 \
  X(0xc0000022, STATUS_ACCESS_DENIED,                           \
    ERROR_ACCESS_DENIED,                                        \
    WSAEACCES, EACCES)                                          \
  X(0xc0000023, STATUS_BUFFER_TOO_SMALL,                        \
    ERROR_INSUFFICIENT_BUFFER,                                  \
    WSAEFAULT, EFAULT)                                          \
  X(0xc0000024, STATUS_OBJECT_TYPE_MISMATCH,                    \
    ERROR_INVALID_HANDLE,                                       \
    WSAENOTSOCK, ENOTSOCK)                                      \
  X(0xc0000027, STATUS_UNWIND,                                  \
    ERROR_UNWIND,                                               \
    -1, -1)                                                     \
  X(0xc0000028, STATUS_BAD_STACK,                               \
    ERROR_BAD_STACK,                                            \
    -1, -1)                                                     \
  X(0xc0000029, STATUS_INVALID_UNWIND_TARGET,                   \
    ERROR_INVALID_UNWIND_TARGET,                                \
    -1, -1)                                                     \
  X(0xc000002a, STATUS_NOT_LOCKED,                              \
    ERROR_NOT_LOCKED,                                           \
    -1, EACCES)                                                 \
  X(0xc000002c, STATUS_UNABLE_TO_DECOMMIT_VM,                   \
    ERROR_INVALID_ADDRESS,                                      \
    -1, -1)                                                     \
  X(0xc000002d, STATUS_NOT_COMMITTED,                           \
    ERROR_INVALID_ADDRESS,                                      \
    -1, -1)                                                     \
  X(0xc000002e, STATUS_INVALID_PORT_ATTRIBUTES,                 \
    ERROR_INVALID_PORT_ATTRIBUTES,                              \
    -1, -1)                                                     \
  X(0xc000002f, STATUS_PORT_MESSAGE_TOO_LONG,                   \
    ERROR_PORT_MESSAGE_TOO_LONG,                                \
    -1, -1)                                                     \
  X(0xc0000030, STATUS_INVALID_PARAMETER_MIX,                   \
    ERROR_INVALID_PARAMETER,                                    \
    -1, EINVAL)                                                 \
  X(0xc0000031, STATUS_INVALID_QUOTA_LOWER,                     \
    ERROR_INVALID_QUOTA_LOWER,                                  \
    -1, -1)                                                     \
  X(0xc0000032, STATUS_DISK_CORRUPT_ERROR,                      \
    ERROR_DISK_CORRUPT,                                         \
    -1, -1)                                                     \
  X(0xc0000033, STATUS_OBJECT_NAME_INVALID,                     \
    ERROR_INVALID_NAME,                                         \
    -1, -1)                                                     \
  X(0xc0000034, STATUS_OBJECT_NAME_NOT_FOUND,                   \
    ERROR_FILE_NOT_FOUND,                                       \
    WSAENETDOWN, ENETDOWN)                                      \
  X(0xc0000035, STATUS_OBJECT_NAME_COLLISION,                   \
    ERROR_ALREADY_EXISTS,                                       \
    -1, EEXIST)                                                 \
  X(0xc0000037, STATUS_PORT_DISCONNECTED,                       \
    ERROR_INVALID_HANDLE,                                       \
    -1, EBADF)                                                  \
  X(0xc0000038, STATUS_DEVICE_ALREADY_ATTACHED,                 \
    ERROR_DEVICE_ALREADY_ATTACHED,                              \
    -1, -1)                                                     \
  X(0xc0000039, STATUS_OBJECT_PATH_INVALID,                     \
    ERROR_BAD_PATHNAME,                                         \
    -1, ENOENT)                                                 \
  X(0xc000003a, STATUS_OBJECT_PATH_NOT_FOUND,                   \
    ERROR_PATH_NOT_FOUND,                                       \
    WSAENETDOWN, ENETDOWN)                                      \
  X(0xc000003b, STATUS_OBJECT_PATH_SYNTAX_BAD,                  \
    ERROR_BAD_PATHNAME,                                         \
    -1, ENOENT)                                                 \
  X(0xc000003c, STATUS_DATA_OVERRUN,                            \
    ERROR_IO_DEVICE,                                            \
    -1, -1)                                                     \
  X(0xc000003d, STATUS_DATA_LATE_ERROR,                         \
    ERROR_IO_DEVICE,                                            \
    -1, -1)                                                     \
  X(0xc000003e, STATUS_DATA_ERROR,                              \
    ERROR_CRC,                                                  \
    -1, EACCES)                                                 \
  X(0xc000003f, STATUS_CRC_ERROR,                               \
    ERROR_CRC,                                                  \
    -1, EACCES)                                                 \
  X(0xc0000040, STATUS_SECTION_TOO_BIG,                         \
    ERROR_NOT_ENOUGH_MEMORY,                                    \
    -1, ENOMEM)                                                 \
  X(0xc0000041, STATUS_PORT_CONNECTION_REFUSED,                 \
    ERROR_ACCESS_DENIED,                                        \
    -1, EACCES)                                                 \
  X(0xc0000042, STATUS_INVALID_PORT_HANDLE,                     \
    ERROR_INVALID_HANDLE,                                       \
    -1, EBADF)                                                  \
  X(0xc0000043, STATUS_SHARING_VIOLATION,                       \
    ERROR_SHARING_VIOLATION,                                    \
    WSAEADDRINUSE, EADDRINUSE)                                  \
  X(0xc0000044, STATUS_QUOTA_EXCEEDED,                          \
    ERROR_NOT_ENOUGH_QUOTA,                                     \
    WSAENOBUFS, ENOBUFS)                                        \
  X(0xc0000045, STATUS_INVALID_PAGE_PROTECTION,                 \
    ERROR_INVALID_PARAMETER,                                    \
    -1, EINVAL)                                                 \
  X(0xc0000046, STATUS_MUTANT_NOT_OWNED,                        \
    ERROR_NOT_OWNER,                                            \
    -1, -1)                                                     \
  X(0xc0000047, STATUS_SEMAPHORE_LIMIT_EXCEEDED,                \
    ERROR_TOO_MANY_POSTS,                                       \
    -1, -1)                                                     \
  X(0xc0000048, STATUS_PORT_ALREADY_SET,                        \
    ERROR_INVALID_PARAMETER,                                    \
    -1, EINVAL)                                                 \
  X(0xc0000049, STATUS_SECTION_NOT_IMAGE,                       \
    ERROR_INVALID_PARAMETER,                                    \
    -1, EINVAL)                                                 \
  X(0xc000004a, STATUS_SUSPEND_COUNT_EXCEEDED,                  \
    ERROR_SIGNAL_REFUSED,                                       \
    -1, -1)                                                     \
  X(0xc000004b, STATUS_THREAD_IS_TERMINATING,                   \
    ERROR_ACCESS_DENIED,                                        \
    -1, EACCES)                                                 \
  X(0xc000004c, STATUS_BAD_WORKING_SET_LIMIT,                   \
    ERROR_INVALID_PARAMETER,                                    \
    -1, EINVAL)                                                 \
  X(0xc000004d, STATUS_INCOMPATIBLE_FILE_MAP,                   \
    ERROR_INVALID_PARAMETER,                                    \
    -1, EINVAL)                                                 \
  X(0xc000004e, STATUS_SECTION_PROTECTION,                      \
    ERROR_INVALID_PARAMETER,                                    \
    -1, EINVAL)                                                 \
  X(0xc000004f, STATUS_EAS_NOT_SUPPORTED,                       \
    ERROR_EAS_NOT_SUPPORTED,                                    \
    -1, -1)                                                     \
  X(0xc0000050, STATUS_EA_TOO_LARGE,                            \
    ERROR_EA_LIST_INCONSISTENT,                                 \
    -1, -1)                                                     \
  X(0xc0000051, STATUS_NONEXISTENT_EA_ENTRY,                    \
    ERROR_FILE_CORRUPT,                                         \
    -1, -1)                                                     \
  X(0xc0000052, STATUS_NO_EAS_ON_FILE,                          \
    ERROR_FILE_CORRUPT,                                         \
    -1, -1)                                                     \
  X(0xc0000053, STATUS_EA_CORRUPT_ERROR,                        \
    ERROR_FILE_CORRUPT,                                         \
    -1, -1)                                                     \
  X(0xc0000054, STATUS_FILE_LOCK_CONFLICT,                      \
    ERROR_LOCK_VIOLATION,                                       \
    -1, EACCES)                                                 \
  X(0xc0000055, STATUS_LOCK_NOT_GRANTED,                        \
    ERROR_LOCK_VIOLATION,                                       \
    -1, EACCES)                                                 \
  X(0xc0000056, STATUS_DELETE_PENDING,                          \
    ERROR_ACCESS_DENIED,                                        \
    -1, EACCES)                                                 \
  X(0xc0000057, STATUS_CTL_FILE_NOT_SUPPORTED,                  \
    ERROR_NOT_SUPPORTED,                                        \
    -1, -1)                                                     \
  X(0xc0000058, STATUS_UNKNOWN_REVISION,                        \
    ERROR_UNKNOWN_REVISION,                                     \
    -1, -1)                                                     \
  X(0xc0000059, STATUS_REVISION_MISMATCH,                       \
    ERROR_REVISION_MISMATCH,                                    \
    -1, -1)                                                     \
  X(0xc000005a, STATUS_INVALID_OWNER,                           \
    ERROR_INVALID_OWNER,                                        \
    -1, -1)                                                     \
  X(0xc000005b, STATUS_INVALID_PRIMARY_GROUP,                   \
    ERROR_INVALID_PRIMARY_GROUP,                                \
    -1, -1)                                                     \
  X(0xc000005c, STATUS_NO_IMPERSONATION_TOKEN,                  \
    ERROR_NO_IMPERSONATION_TOKEN,                               \
    -1, -1)                                                     \
  X(0xc000005d, STATUS_CANT_DISABLE_MANDATORY,                  \
    ERROR_CANT_DISABLE_MANDATORY,                               \
    -1, -1)                                                     \
  X(0xc000005e, STATUS_NO_LOGON_SERVERS,                        \
    ERROR_NO_LOGON_SERVERS,                                     \
    -1, -1)                                                     \
  X(0xc000005f, STATUS_NO_SUCH_LOGON_SESSION,                   \
    ERROR_NO_SUCH_LOGON_SESSION,                                \
    -1, -1)                                                     \
  X(0xc0000060, STATUS_NO_SUCH_PRIVILEGE,                       \
    ERROR_NO_SUCH_PRIVILEGE,                                    \
    -1, -1)                                                     \
  X(0xc0000061, STATUS_PRIVILEGE_NOT_HELD,                      \
    ERROR_PRIVILEGE_NOT_HELD,                                   \
    -1, -1)                                                     \
  X(0xc0000062, STATUS_INVALID_ACCOUNT_NAME,                    \
    ERROR_INVALID_ACCOUNT_NAME,                                 \
    -1, -1)                                                     \
  X(0xc0000063, STATUS_USER_EXISTS,                             \
    ERROR_USER_EXISTS,                                          \
    -1, -1)                                                     \
  X(0xc0000064, STATUS_NO_SUCH_USER,                            \
    ERROR_NO_SUCH_USER,                                         \
    -1, -1)                                                     \
  X(0xc0000065, STATUS_GROUP_EXISTS,                            \
    ERROR_GROUP_EXISTS,                                         \
    -1, -1)                                                     \
  X(0xc0000066, STATUS_NO_SUCH_GROUP,                           \
    ERROR_NO_SUCH_GROUP,                                        \
    -1, -1)                                                     \
  X(0xc0000067, STATUS_MEMBER_IN_GROUP,                         \
    ERROR_MEMBER_IN_GROUP,                                      \
    -1, -1)                                                     \
  X(0xc0000068, STATUS_MEMBER_NOT_IN_GROUP,                     \
    ERROR_MEMBER_NOT_IN_GROUP,                                  \
    -1, -1)                                                     \
  X(0xc0000069, STATUS_LAST_ADMIN,                              \
    ERROR_LAST_ADMIN,                                           \
    -1, -1)                                                     \
  X(0xc000006a, STATUS_WRONG_PASSWORD,                          \
    ERROR_INVALID_PASSWORD,                                     \
    -1, -1)                                                     \
  X(0xc000006b, STATUS_ILL_FORMED_PASSWORD,                     \
    ERROR_ILL_FORMED_PASSWORD,                                  \
    -1, -1)                                                     \
  X(0xc000006c, STATUS_PASSWORD_RESTRICTION,                    \
    ERROR_PASSWORD_RESTRICTION,                                 \
    -1, -1)                                                     \
  X(0xc000006d, STATUS_LOGON_FAILURE,                           \
    ERROR_LOGON_FAILURE,                                        \
    -1, -1)                                                     \
  X(0xc000006e, STATUS_ACCOUNT_RESTRICTION,                     \
    ERROR_ACCOUNT_RESTRICTION,                                  \
    -1, -1)                                                     \
  X(0xc000006f, STATUS_INVALID_LOGON_HOURS,                     \
    ERROR_INVALID_LOGON_HOURS,                                  \
    -1, -1)                                                     \
  X(0xc0000070, STATUS_INVALID_WORKSTATION,                     \
    ERROR_INVALID_WORKSTATION,                                  \
    -1, -1)                                                     \
  X(0xc0000071, STATUS_PASSWORD_EXPIRED,                        \
    ERROR_PASSWORD_EXPIRED,                                     \
    -1, -1)                                                     \
  X(0xc0000072, STATUS_ACCOUNT_DISABLED,                        \
    ERROR_ACCOUNT_DISABLED,                                     \
    -1, -1)                                                     \
  X(0xc0000073, STATUS_NONE_MAPPED,                             \
    ERROR_NONE_MAPPED,                                          \
    -1, -1)                                                     \
  X(0xc0000074, STATUS_TOO_MANY_LUIDS_REQUESTED,                \
    ERROR_TOO_MANY_LUIDS_REQUESTED,                             \
    -1, -1)                                                     \
  X(0xc0000075, STATUS_LUIDS_EXHAUSTED,                         \
    ERROR_LUIDS_EXHAUSTED,                                      \
    -1, -1)                                                     \
  X(0xc0000076, STATUS_INVALID_SUB_AUTHORITY,                   \
    ERROR_INVALID_SUB_AUTHORITY,                                \
    -1, -1)                                                     \
  X(0xc0000077, STATUS_INVALID_ACL,                             \
    ERROR_INVALID_ACL,                                          \
    -1, -1)                                                     \
  X(0xc0000078, STATUS_INVALID_SID,                             \
    ERROR_INVALID_SID,                                          \
    -1, -1)                                                     \
  X(0xc0000079, STATUS_INVALID_SECURITY_DESCR,                  \
    ERROR_INVALID_SECURITY_DESCR,                               \
    -1, -1)                                                     \
  X(0xc000007a, STATUS_PROCEDURE_NOT_FOUND,                     \
    ERROR_PROC_NOT_FOUND,                                       \
    -1, -1)                                                     \
  X(0xc000007b, STATUS_INVALID_IMAGE_FORMAT,                    \
    ERROR_BAD_EXE_FORMAT,                                       \
    -1, ENOEXEC)                                                \
  X(0xc000007c, STATUS_NO_TOKEN,                                \
    ERROR_NO_TOKEN,                                             \
    -1, -1)                                                     \
  X(0xc000007d, STATUS_BAD_INHERITANCE_ACL,                     \
    ERROR_BAD_INHERITANCE_ACL,                                  \
    -1, -1)                                                     \
  X(0xc000007e, STATUS_RANGE_NOT_LOCKED,                        \
    ERROR_NOT_LOCKED,                                           \
    -1, EACCES)                                                 \
  X(0xc000007f, STATUS_DISK_FULL,                               \
    ERROR_DISK_FULL,                                            \
    -1, ENOSPC)                                                 \
  X(0xc0000080, STATUS_SERVER_DISABLED,                         \
    ERROR_SERVER_DISABLED,                                      \
    -1, -1)                                                     \
  X(0xc0000081, STATUS_SERVER_NOT_DISABLED,                     \
    ERROR_SERVER_NOT_DISABLED,                                  \
    -1, -1)                                                     \
  X(0xc0000082, STATUS_TOO_MANY_GUIDS_REQUESTED,                \
    ERROR_TOO_MANY_NAMES,                                       \
    -1, -1)                                                     \
  X(0xc0000083, STATUS_GUIDS_EXHAUSTED,                         \
    ERROR_NO_MORE_ITEMS,                                        \
    -1, -1)                                                     \
  X(0xc0000084, STATUS_INVALID_ID_AUTHORITY,                    \
    ERROR_INVALID_ID_AUTHORITY,                                 \
    -1, -1)                                                     \
  X(0xc0000085, STATUS_AGENTS_EXHAUSTED,                        \
    ERROR_NO_MORE_ITEMS,                                        \
    -1, -1)                                                     \
  X(0xc0000086, STATUS_INVALID_VOLUME_LABEL,                    \
    ERROR_LABEL_TOO_LONG,                                       \
    -1, -1)                                                     \
  X(0xc0000087, STATUS_SECTION_NOT_EXTENDED,                    \
    ERROR_OUTOFMEMORY,                                          \
    -1, -1)                                                     \
  X(0xc0000088, STATUS_NOT_MAPPED_DATA,                         \
    ERROR_INVALID_ADDRESS,                                      \
    -1, -1)                                                     \
  X(0xc0000089, STATUS_RESOURCE_DATA_NOT_FOUND,                 \
    ERROR_RESOURCE_DATA_NOT_FOUND,                              \
    -1, -1)                                                     \
  X(0xc000008a, STATUS_RESOURCE_TYPE_NOT_FOUND,                 \
    ERROR_RESOURCE_TYPE_NOT_FOUND,                              \
    -1, -1)                                                     \
  X(0xc000008b, STATUS_RESOURCE_NAME_NOT_FOUND,                 \
    ERROR_RESOURCE_NAME_NOT_FOUND,                              \
    -1, -1)                                                     \
  X(0xc0000095, STATUS_INTEGER_OVERFLOW,                        \
    ERROR_ARITHMETIC_OVERFLOW,                                  \
    -1, -1)                                                     \
  X(0xc0000097, STATUS_TOO_MANY_PAGING_FILES,                   \
    ERROR_NOT_ENOUGH_MEMORY,                                    \
    WSAENOBUFS, ENOBUFS)                                        \
  X(0xc0000098, STATUS_FILE_INVALID,                            \
    ERROR_FILE_INVALID,                                         \
    -1, -1)                                                     \
  X(0xc0000099, STATUS_ALLOTTED_SPACE_EXCEEDED,                 \
    ERROR_ALLOTTED_SPACE_EXCEEDED,                              \
    -1, -1)                                                     \
  X(0xc000009a, STATUS_INSUFFICIENT_RESOURCES,                  \
    ERROR_NO_SYSTEM_RESOURCES,                                  \
    WSAENOBUFS, ENOBUFS)                                        \
  X(0xc000009b, STATUS_DFS_EXIT_PATH_FOUND,                     \
    ERROR_PATH_NOT_FOUND,                                       \
    -1, ENOENT)                                                 \
  X(0xc000009c, STATUS_DEVICE_DATA_ERROR,                       \
    ERROR_CRC,                                                  \
    -1, EACCES)                                                 \
  X(0xc000009d, STATUS_DEVICE_NOT_CONNECTED,                    \
    ERROR_DEVICE_NOT_CONNECTED,                                 \
    -1, -1)                                                     \
  X(0xc000009e, STATUS_DEVICE_POWER_FAILURE,                    \
    ERROR_NOT_READY,                                            \
    -1, EACCES)                                                 \
  X(0xc000009f, STATUS_FREE_VM_NOT_AT_BASE,                     \
    ERROR_INVALID_ADDRESS,                                      \
    -1, -1)                                                     \
  X(0xc00000a0, STATUS_MEMORY_NOT_ALLOCATED,                    \
    ERROR_INVALID_ADDRESS,                                      \
    -1, -1)                                                     \
  X(0xc00000a1, STATUS_WORKING_SET_QUOTA,                       \
    ERROR_WORKING_SET_QUOTA,                                    \
    WSAENOBUFS, ENOBUFS)                                        \
  X(0xc00000a2, STATUS_MEDIA_WRITE_PROTECTED,                   \
    ERROR_WRITE_PROTECT,                                        \
    -1, EACCES)                                                 \
  X(0xc00000a3, STATUS_DEVICE_NOT_READY,                        \
    ERROR_NOT_READY,                                            \
    WSAEWOULDBLOCK, EWOULDBLOCK)                                \
  X(0xc00000a4, STATUS_INVALID_GROUP_ATTRIBUTES,                \
    ERROR_INVALID_GROUP_ATTRIBUTES,                             \
    -1, -1)                                                     \
  X(0xc00000a5, STATUS_BAD_IMPERSONATION_LEVEL,                 \
    ERROR_BAD_IMPERSONATION_LEVEL,                              \
    -1, -1)                                                     \
  X(0xc00000a6, STATUS_CANT_OPEN_ANONYMOUS,                     \
    ERROR_CANT_OPEN_ANONYMOUS,                                  \
    -1, -1)                                                     \
  X(0xc00000a7, STATUS_BAD_VALIDATION_CLASS,                    \
    ERROR_BAD_VALIDATION_CLASS,                                 \
    -1, -1)                                                     \
  X(0xc00000a8, STATUS_BAD_TOKEN_TYPE,                          \
    ERROR_BAD_TOKEN_TYPE,                                       \
    -1, -1)                                                     \
  X(0xc00000a9, STATUS_BAD_MASTER_BOOT_RECORD,                  \
    ERROR_INVALID_PARAMETER,                                    \
    -1, EINVAL)                                                 \
  X(0xc00000aa, STATUS_INSTRUCTION_MISALIGNMENT,                \
    ERROR_INSTRUCTION_MISALIGNMENT,                             \
    -1, -1)                                                     \
  X(0xc00000ab, STATUS_INSTANCE_NOT_AVAILABLE,                  \
    ERROR_PIPE_BUSY,                                            \
    -1, -1)                                                     \
  X(0xc00000ac, STATUS_PIPE_NOT_AVAILABLE,                      \
    ERROR_PIPE_BUSY,                                            \
    -1, -1)                                                     \
  X(0xc00000ad, STATUS_INVALID_PIPE_STATE,                      \
    ERROR_BAD_PIPE,                                             \
    -1, -1)                                                     \
  X(0xc00000ae, STATUS_PIPE_BUSY,                               \
    ERROR_PIPE_BUSY,                                            \
    -1, -1)                                                     \
  X(0xc00000af, STATUS_ILLEGAL_FUNCTION,                        \
    ERROR_INVALID_FUNCTION,                                     \
    -1, EINVAL)                                                 \
  X(0xc00000b0, STATUS_PIPE_DISCONNECTED,                       \
    ERROR_PIPE_NOT_CONNECTED,                                   \
    WSAESHUTDOWN, EPIPE)                                        \
  X(0xc00000b1, STATUS_PIPE_CLOSING,                            \
    ERROR_NO_DATA,                                              \
    -1, -1)                                                     \
  X(0xc00000b2, STATUS_PIPE_CONNECTED,                          \
    ERROR_PIPE_CONNECTED,                                       \
    -1, -1)                                                     \
  X(0xc00000b3, STATUS_PIPE_LISTENING,                          \
    ERROR_PIPE_LISTENING,                                       \
    -1, -1)                                                     \
  X(0xc00000b4, STATUS_INVALID_READ_MODE,                       \
    ERROR_BAD_PIPE,                                             \
    -1, -1)                                                     \
  X(0xc00000b5, STATUS_IO_TIMEOUT,                              \
    ERROR_SEM_TIMEOUT,                                          \
    WSAETIMEDOUT, ETIMEDOUT)                                    \
  X(0xc00000b6, STATUS_FILE_FORCED_CLOSED,                      \
    ERROR_HANDLE_EOF,                                           \
    -1, -1)                                                     \
  X(0xc00000b7, STATUS_PROFILING_NOT_STARTED,                   \
    ERROR_PROFILING_NOT_STARTED,                                \
    -1, -1)                                                     \
  X(0xc00000b8, STATUS_PROFILING_NOT_STOPPED,                   \
    ERROR_PROFILING_NOT_STOPPED,                                \
    -1, -1)                                                     \
  X(0xc00000b9, STATUS_COULD_NOT_INTERPRET,                     \
    ERROR_COULD_NOT_INTERPRET,                                  \
    -1, -1)                                                     \
  X(0xc00000ba, STATUS_FILE_IS_A_DIRECTORY,                     \
    ERROR_ACCESS_DENIED,                                        \
    -1, EACCES)                                                 \
  X(0xc00000bb, STATUS_NOT_SUPPORTED,                           \
    ERROR_NOT_SUPPORTED,                                        \
    WSAEOPNOTSUPP, EOPNOTSUPP)                                  \
  X(0xc00000bc, STATUS_REMOTE_NOT_LISTENING,                    \
    ERROR_REM_NOT_LIST,                                         \
    WSAECONNREFUSED, ECONNREFUSED)                              \
  X(0xc00000bd, STATUS_DUPLICATE_NAME,                          \
    ERROR_DUP_NAME,                                             \
    -1, -1)                                                     \
  X(0xc00000be, STATUS_BAD_NETWORK_PATH,                        \
    ERROR_BAD_NETPATH,                                          \
    WSAENETUNREACH, ENETUNREACH)                                \
  X(0xc00000bf, STATUS_NETWORK_BUSY,                            \
    ERROR_NETWORK_BUSY,                                         \
    WSAENETDOWN, ENETDOWN)                                      \
  X(0xc00000c0, STATUS_DEVICE_DOES_NOT_EXIST,                   \
    ERROR_DEV_NOT_EXIST,                                        \
    -1, -1)                                                     \
  X(0xc00000c1, STATUS_TOO_MANY_COMMANDS,                       \
    ERROR_TOO_MANY_CMDS,                                        \
    -1, -1)                                                     \
  X(0xc00000c2, STATUS_ADAPTER_HARDWARE_ERROR,                  \
    ERROR_ADAP_HDW_ERR,                                         \
    -1, -1)                                                     \
  X(0xc00000c3, STATUS_INVALID_NETWORK_RESPONSE,                \
    ERROR_BAD_NET_RESP,                                         \
    WSAENETDOWN, ENETDOWN)                                      \
  X(0xc00000c4, STATUS_UNEXPECTED_NETWORK_ERROR,                \
    ERROR_UNEXP_NET_ERR,                                        \
    WSAENETDOWN, ENETDOWN)                                      \
  X(0xc00000c5, STATUS_BAD_REMOTE_ADAPTER,                      \
    ERROR_BAD_REM_ADAP,                                         \
    -1, -1)                                                     \
  X(0xc00000c6, STATUS_PRINT_QUEUE_FULL,                        \
    ERROR_PRINTQ_FULL,                                          \
    -1, -1)                                                     \
  X(0xc00000c7, STATUS_NO_SPOOL_SPACE,                          \
    ERROR_NO_SPOOL_SPACE,                                       \
    -1, -1)                                                     \
  X(0xc00000c8, STATUS_PRINT_CANCELLED,                         \
    ERROR_PRINT_CANCELLED,                                      \
    -1, -1)                                                     \
  X(0xc00000c9, STATUS_NETWORK_NAME_DELETED,                    \
    ERROR_NETNAME_DELETED,                                      \
    -1, -1)                                                     \
  X(0xc00000ca, STATUS_NETWORK_ACCESS_DENIED,                   \
    ERROR_NETWORK_ACCESS_DENIED,                                \
    -1, EACCES)                                                 \
  X(0xc00000cb, STATUS_BAD_DEVICE_TYPE,                         \
    ERROR_BAD_DEV_TYPE,                                         \
    -1, -1)                                                     \
  X(0xc00000cc, STATUS_BAD_NETWORK_NAME,                        \
    ERROR_BAD_NET_NAME,                                         \
    -1, ENOENT)                                                 \
  X(0xc00000cd, STATUS_TOO_MANY_NAMES,                          \
    ERROR_TOO_MANY_NAMES,                                       \
    -1, -1)                                                     \
  X(0xc00000ce, STATUS_TOO_MANY_SESSIONS,                       \
    ERROR_TOO_MANY_SESS,                                        \
    -1, -1)                                                     \
  X(0xc00000cf, STATUS_SHARING_PAUSED,                          \
    ERROR_SHARING_PAUSED,                                       \
    -1, -1)                                                     \
  X(0xc00000d0, STATUS_REQUEST_NOT_ACCEPTED,                    \
    ERROR_REQ_NOT_ACCEP,                                        \
    WSAEWOULDBLOCK, EWOULDBLOCK)                                \
  X(0xc00000d1, STATUS_REDIRECTOR_PAUSED,                       \
    ERROR_REDIR_PAUSED,                                         \
    -1, -1)                                                     \
  X(0xc00000d2, STATUS_NET_WRITE_FAULT,                         \
    ERROR_NET_WRITE_FAULT,                                      \
    -1, -1)                                                     \
  X(0xc00000d3, STATUS_PROFILING_AT_LIMIT,                      \
    ERROR_PROFILING_AT_LIMIT,                                   \
    -1, -1)                                                     \
  X(0xc00000d4, STATUS_NOT_SAME_DEVICE,                         \
    ERROR_NOT_SAME_DEVICE,                                      \
    -1, EXDEV)                                                  \
  X(0xc00000d5, STATUS_FILE_RENAMED,                            \
    ERROR_ACCESS_DENIED,                                        \
    -1, EACCES)                                                 \
  X(0xc00000d6, STATUS_VIRTUAL_CIRCUIT_CLOSED,                  \
    ERROR_VC_DISCONNECTED,                                      \
    -1, -1)                                                     \
  X(0xc00000d7, STATUS_NO_SECURITY_ON_OBJECT,                   \
    ERROR_NO_SECURITY_ON_OBJECT,                                \
    -1, -1)                                                     \
  X(0xc00000d8, STATUS_CANT_WAIT,                               \
    ERROR_CANT_WAIT,                                            \
    -1, -1)                                                     \
  X(0xc00000d9, STATUS_PIPE_EMPTY,                              \
    ERROR_NO_DATA,                                              \
    -1, -1)                                                     \
  X(0xc00000da, STATUS_CANT_ACCESS_DOMAIN_INFO,                 \
    ERROR_CANT_ACCESS_DOMAIN_INFO,                              \
    -1, -1)                                                     \
  X(0xc00000db, STATUS_CANT_TERMINATE_SELF,                     \
    ERROR_CANT_TERMINATE_SELF,                                  \
    -1, -1)                                                     \
  X(0xc00000dc, STATUS_INVALID_SERVER_STATE,                    \
    ERROR_INVALID_SERVER_STATE,                                 \
    -1, -1)                                                     \
  X(0xc00000dd, STATUS_INVALID_DOMAIN_STATE,                    \
    ERROR_INVALID_DOMAIN_STATE,                                 \
    -1, -1)                                                     \
  X(0xc00000de, STATUS_INVALID_DOMAIN_ROLE,                     \
    ERROR_INVALID_DOMAIN_ROLE,                                  \
    -1, -1)                                                     \
  X(0xc00000df, STATUS_NO_SUCH_DOMAIN,                          \
    ERROR_NO_SUCH_DOMAIN,                                       \
    -1, -1)                                                     \
  X(0xc00000e0, STATUS_DOMAIN_EXISTS,                           \
    ERROR_DOMAIN_EXISTS,                                        \
    -1, -1)                                                     \
  X(0xc00000e1, STATUS_DOMAIN_LIMIT_EXCEEDED,                   \
    ERROR_DOMAIN_LIMIT_EXCEEDED,                                \
    -1, -1)                                                     \
  X(0xc00000e2, STATUS_OPLOCK_NOT_GRANTED,                      \
    ERROR_OPLOCK_NOT_GRANTED,                                   \
    -1, -1)                                                     \
  X(0xc00000e3, STATUS_INVALID_OPLOCK_PROTOCOL,                 \
    ERROR_INVALID_OPLOCK_PROTOCOL,                              \
    -1, -1)                                                     \
  X(0xc00000e4, STATUS_INTERNAL_DB_CORRUPTION,                  \
    ERROR_INTERNAL_DB_CORRUPTION,                               \
    -1, -1)                                                     \
  X(0xc00000e5, STATUS_INTERNAL_ERROR,                          \
    ERROR_INTERNAL_ERROR,                                       \
    -1, -1)                                                     \
  X(0xc00000e6, STATUS_GENERIC_NOT_MAPPED,                      \
    ERROR_GENERIC_NOT_MAPPED,                                   \
    -1, -1)                                                     \
  X(0xc00000e7, STATUS_BAD_DESCRIPTOR_FORMAT,                   \
    ERROR_BAD_DESCRIPTOR_FORMAT,                                \
    -1, -1)                                                     \
  X(0xc00000e8, STATUS_INVALID_USER_BUFFER,                     \
    ERROR_INVALID_USER_BUFFER,                                  \
    -1, -1)                                                     \
  X(0xc00000e9, STATUS_UNEXPECTED_IO_ERROR,                     \
    ERROR_IO_DEVICE,                                            \
    -1, -1)                                                     \
  X(0xc00000ea, STATUS_UNEXPECTED_MM_CREATE_ERR,                \
    ERROR_UNEXPECTED_MM_CREATE_ERR,                             \
    -1, -1)                                                     \
  X(0xc00000eb, STATUS_UNEXPECTED_MM_MAP_ERROR,                 \
    ERROR_UNEXPECTED_MM_MAP_ERROR,                              \
    -1, -1)                                                     \
  X(0xc00000ec, STATUS_UNEXPECTED_MM_EXTEND_ERR,                \
    ERROR_UNEXPECTED_MM_EXTEND_ERR,                             \
    -1, -1)                                                     \
  X(0xc00000ed, STATUS_NOT_LOGON_PROCESS,                       \
    ERROR_NOT_LOGON_PROCESS,                                    \
    -1, -1)                                                     \
  X(0xc00000ee, STATUS_LOGON_SESSION_EXISTS,                    \
    ERROR_LOGON_SESSION_EXISTS,                                 \
    -1, -1)                                                     \
  X(0xc00000ef, STATUS_INVALID_PARAMETER_1,                     \
    ERROR_INVALID_PARAMETER,                                    \
    -1, EINVAL)                                                 \
  X(0xc00000f0, STATUS_INVALID_PARAMETER_2,                     \
    ERROR_INVALID_PARAMETER,                                    \
    -1, EINVAL)                                                 \
  X(0xc00000f1, STATUS_INVALID_PARAMETER_3,                     \
    ERROR_INVALID_PARAMETER,                                    \
    -1, EINVAL)                                                 \
  X(0xc00000f2, STATUS_INVALID_PARAMETER_4,                     \
    ERROR_INVALID_PARAMETER,                                    \
    -1, EINVAL)                                                 \
  X(0xc00000f3, STATUS_INVALID_PARAMETER_5,                     \
    ERROR_INVALID_PARAMETER,                                    \
    -1, EINVAL)                                                 \
  X(0xc00000f4, STATUS_INVALID_PARAMETER_6,                     \
    ERROR_INVALID_PARAMETER,                                    \
    -1, EINVAL)                                                 \
  X(0xc00000f5, STATUS_INVALID_PARAMETER_7,                     \
    ERROR_INVALID_PARAMETER,                                    \
    -1, EINVAL)                                                 \
  X(0xc00000f6, STATUS_INVALID_PARAMETER_8,                     \
    ERROR_INVALID_PARAMETER,                                    \
    -1, EINVAL)                                                 \
  X(0xc00000f7, STATUS_INVALID_PARAMETER_9,                     \
    ERROR_INVALID_PARAMETER,                                    \
    -1, EINVAL)                                                 \
  X(0xc00000f8, STATUS_INVALID_PARAMETER_10,                    \
    ERROR_INVALID_PARAMETER,                                    \
    -1, EINVAL)                                                 \
  X(0xc00000f9, STATUS_INVALID_PARAMETER_11,                    \
    ERROR_INVALID_PARAMETER,                                    \
    -1, EINVAL)                                                 \
  X(0xc00000fa, STATUS_INVALID_PARAMETER_12,                    \
    ERROR_INVALID_PARAMETER,                                    \
    -1, EINVAL)                                                 \
  X(0xc00000fb, STATUS_REDIRECTOR_NOT_STARTED,                  \
    ERROR_PATH_NOT_FOUND,                                       \
    -1, ENOENT)                                                 \
  X(0xc00000fc, STATUS_REDIRECTOR_STARTED,                      \
    ERROR_SERVICE_ALREADY_RUNNING,                              \
    -1, -1)                                                     \
  X(0xc00000fd, STATUS_STACK_OVERFLOW,                          \
    ERROR_STACK_OVERFLOW,                                       \
    -1, -1)                                                     \
  X(0xc00000fe, STATUS_NO_SUCH_PACKAGE,                         \
    ERROR_NO_SUCH_PACKAGE,                                      \
    -1, -1)                                                     \
  X(0xc00000ff, STATUS_BAD_FUNCTION_TABLE,                      \
    ERROR_BAD_FUNCTION_TABLE,                                   \
    -1, -1)                                                     \
  X(0xc0000100, STATUS_VARIABLE_NOT_FOUND,                      \
    ERROR_ENVVAR_NOT_FOUND,                                     \
    -1, -1)                                                     \
  X(0xc0000101, STATUS_DIRECTORY_NOT_EMPTY,                     \
    ERROR_DIR_NOT_EMPTY,                                        \
    -1, ENOTEMPTY)                                              \
  X(0xc0000102, STATUS_FILE_CORRUPT_ERROR,                      \
    ERROR_FILE_CORRUPT,                                         \
    -1, -1)                                                     \
  X(0xc0000103, STATUS_NOT_A_DIRECTORY,                         \
    ERROR_DIRECTORY,                                            \
    -1, -1)                                                     \
  X(0xc0000104, STATUS_BAD_LOGON_SESSION_STATE,                 \
    ERROR_BAD_LOGON_SESSION_STATE,                              \
    -1, -1)                                                     \
  X(0xc0000105, STATUS_LOGON_SESSION_COLLISION,                 \
    ERROR_LOGON_SESSION_COLLISION,                              \
    -1, -1)                                                     \
  X(0xc0000106, STATUS_NAME_TOO_LONG,                           \
    ERROR_FILENAME_EXCED_RANGE,                                 \
    -1, ENOENT)                                                 \
  X(0xc0000107, STATUS_FILES_OPEN,                              \
    ERROR_OPEN_FILES,                                           \
    -1, -1)                                                     \
  X(0xc0000108, STATUS_CONNECTION_IN_USE,                       \
    ERROR_DEVICE_IN_USE,                                        \
    -1, -1)                                                     \
  X(0xc000010a, STATUS_PROCESS_IS_TERMINATING,                  \
    ERROR_ACCESS_DENIED,                                        \
    -1, EACCES)                                                 \
  X(0xc000010b, STATUS_INVALID_LOGON_TYPE,                      \
    ERROR_INVALID_LOGON_TYPE,                                   \
    -1, -1)                                                     \
  X(0xc000010c, STATUS_NO_GUID_TRANSLATION,                     \
    ERROR_NO_GUID_TRANSLATION,                                  \
    -1, -1)                                                     \
  X(0xc000010d, STATUS_CANNOT_IMPERSONATE,                      \
    ERROR_CANNOT_IMPERSONATE,                                   \
    -1, -1)                                                     \
  X(0xc000010e, STATUS_IMAGE_ALREADY_LOADED,                    \
    ERROR_SERVICE_ALREADY_RUNNING,                              \
    -1, -1)                                                     \
  X(0xc000010f, STATUS_ABIOS_NOT_PRESENT,                       \
    ERROR_ABIOS_ERROR,                                          \
    -1, -1)                                                     \
  X(0xc0000110, STATUS_ABIOS_LID_NOT_EXIST,                     \
    ERROR_ABIOS_ERROR,                                          \
    -1, -1)                                                     \
  X(0xc0000111, STATUS_ABIOS_LID_ALREADY_OWNED,                 \
    ERROR_ABIOS_ERROR,                                          \
    -1, -1)                                                     \
  X(0xc0000112, STATUS_ABIOS_NOT_LID_OWNER,                     \
    ERROR_ABIOS_ERROR,                                          \
    -1, -1)                                                     \
  X(0xc0000113, STATUS_ABIOS_INVALID_COMMAND,                   \
    ERROR_ABIOS_ERROR,                                          \
    -1, -1)                                                     \
  X(0xc0000114, STATUS_ABIOS_INVALID_LID,                       \
    ERROR_ABIOS_ERROR,                                          \
    -1, -1)                                                     \
  X(0xc0000115, STATUS_ABIOS_SELECTOR_NOT_AVAILABLE,            \
    ERROR_ABIOS_ERROR,                                          \
    -1, -1)                                                     \
  X(0xc0000116, STATUS_ABIOS_INVALID_SELECTOR,                  \
    ERROR_ABIOS_ERROR,                                          \
    -1, -1)                                                     \
  X(0xc0000117, STATUS_NO_LDT,                                  \
    ERROR_INVALID_THREAD_ID,                                    \
    -1, -1)                                                     \
  X(0xc0000118, STATUS_INVALID_LDT_SIZE,                        \
    ERROR_INVALID_LDT_SIZE,                                     \
    -1, -1)                                                     \
  X(0xc0000119, STATUS_INVALID_LDT_OFFSET,                      \
    ERROR_INVALID_LDT_OFFSET,                                   \
    -1, -1)                                                     \
  X(0xc000011a, STATUS_INVALID_LDT_DESCRIPTOR,                  \
    ERROR_INVALID_LDT_DESCRIPTOR,                               \
    -1, -1)                                                     \
  X(0xc000011b, STATUS_INVALID_IMAGE_NE_FORMAT,                 \
    ERROR_BAD_EXE_FORMAT,                                       \
    -1, ENOEXEC)                                                \
  X(0xc000011c, STATUS_RXACT_INVALID_STATE,                     \
    ERROR_RXACT_INVALID_STATE,                                  \
    -1, -1)                                                     \
  X(0xc000011d, STATUS_RXACT_COMMIT_FAILURE,                    \
    ERROR_RXACT_COMMIT_FAILURE,                                 \
    -1, -1)                                                     \
  X(0xc000011e, STATUS_MAPPED_FILE_SIZE_ZERO,                   \
    ERROR_FILE_INVALID,                                         \
    -1, -1)                                                     \
  X(0xc000011f, STATUS_TOO_MANY_OPENED_FILES,                   \
    ERROR_TOO_MANY_OPEN_FILES,                                  \
    -1, EMFILE)                                                 \
  X(0xc0000120, STATUS_CANCELLED,                               \
    ERROR_OPERATION_ABORTED,                                    \
    WSAEINTR, EINTR)                                            \
  X(0xc0000121, STATUS_CANNOT_DELETE,                           \
    ERROR_ACCESS_DENIED,                                        \
    -1, EACCES)                                                 \
  X(0xc0000122, STATUS_INVALID_COMPUTER_NAME,                   \
    ERROR_INVALID_COMPUTERNAME,                                 \
    -1, -1)                                                     \
  X(0xc0000123, STATUS_FILE_DELETED,                            \
    ERROR_ACCESS_DENIED,                                        \
    -1, EACCES)                                                 \
  X(0xc0000124, STATUS_SPECIAL_ACCOUNT,                         \
    ERROR_SPECIAL_ACCOUNT,                                      \
    -1, -1)                                                     \
  X(0xc0000125, STATUS_SPECIAL_GROUP,                           \
    ERROR_SPECIAL_GROUP,                                        \
    -1, -1)                                                     \
  X(0xc0000126, STATUS_SPECIAL_USER,                            \
    ERROR_SPECIAL_USER,                                         \
    -1, -1)                                                     \
  X(0xc0000127, STATUS_MEMBERS_PRIMARY_GROUP,                   \
    ERROR_MEMBERS_PRIMARY_GROUP,                                \
    -1, -1)                                                     \
  X(0xc0000128, STATUS_FILE_CLOSED,                             \
    ERROR_INVALID_HANDLE,                                       \
    -1, EBADF)                                                  \
  X(0xc0000129, STATUS_TOO_MANY_THREADS,                        \
    ERROR_TOO_MANY_THREADS,                                     \
    -1, -1)                                                     \
  X(0xc000012a, STATUS_THREAD_NOT_IN_PROCESS,                   \
    ERROR_THREAD_NOT_IN_PROCESS,                                \
    -1, -1)                                                     \
  X(0xc000012b, STATUS_TOKEN_ALREADY_IN_USE,                    \
    ERROR_TOKEN_ALREADY_IN_USE,                                 \
    -1, -1)                                                     \
  X(0xc000012c, STATUS_PAGEFILE_QUOTA_EXCEEDED,                 \
    ERROR_PAGEFILE_QUOTA_EXCEEDED,                              \
    -1, -1)                                                     \
  X(0xc000012d, STATUS_COMMITMENT_LIMIT,                        \
    ERROR_COMMITMENT_LIMIT,                                     \
    WSAENOBUFS, ENOBUFS)                                        \
  X(0xc000012e, STATUS_INVALID_IMAGE_LE_FORMAT,                 \
    ERROR_BAD_EXE_FORMAT,                                       \
    -1, ENOEXEC)                                                \
  X(0xc000012f, STATUS_INVALID_IMAGE_NOT_MZ,                    \
    ERROR_BAD_EXE_FORMAT,                                       \
    -1, ENOEXEC)                                                \
  X(0xc0000130, STATUS_INVALID_IMAGE_PROTECT,                   \
    ERROR_BAD_EXE_FORMAT,                                       \
    -1, ENOEXEC)                                                \
  X(0xc0000131, STATUS_INVALID_IMAGE_WIN_16,                    \
    ERROR_BAD_EXE_FORMAT,                                       \
    -1, ENOEXEC)                                                \
  X(0xc0000132, STATUS_LOGON_SERVER_CONFLICT,                   \
    ERROR_LOGON_SERVER_CONFLICT,                                \
    -1, -1)                                                     \
  X(0xc0000133, STATUS_TIME_DIFFERENCE_AT_DC,                   \
    ERROR_TIME_SKEW,                                            \
    -1, -1)                                                     \
  X(0xc0000134, STATUS_SYNCHRONIZATION_REQUIRED,                \
    ERROR_SYNCHRONIZATION_REQUIRED,                             \
    -1, -1)                                                     \
  X(0xc0000135, STATUS_DLL_NOT_FOUND,                           \
    ERROR_MOD_NOT_FOUND,                                        \
    -1, -1)                                                     \
  X(0xc0000136, STATUS_OPEN_FAILED,                             \
    ERROR_NET_OPEN_FAILED,                                      \
    -1, -1)                                                     \
  X(0xc0000137, STATUS_IO_PRIVILEGE_FAILED,                     \
    ERROR_IO_PRIVILEGE_FAILED,                                  \
    -1, -1)                                                     \
  X(0xc0000138, STATUS_ORDINAL_NOT_FOUND,                       \
    ERROR_INVALID_ORDINAL,                                      \
    -1, -1)                                                     \
  X(0xc0000139, STATUS_ENTRYPOINT_NOT_FOUND,                    \
    ERROR_PROC_NOT_FOUND,                                       \
    -1, -1)                                                     \
  X(0xc000013a, STATUS_CONTROL_C_EXIT,                          \
    ERROR_CONTROL_C_EXIT,                                       \
    -1, -1)                                                     \
  X(0xc000013b, STATUS_LOCAL_DISCONNECT,                        \
    ERROR_NETNAME_DELETED,                                      \
    WSAECONNABORTED, ECONNABORTED)                              \
  X(0xc000013c, STATUS_REMOTE_DISCONNECT,                       \
    ERROR_NETNAME_DELETED,                                      \
    WSAECONNRESET, ECONNRESET)                                  \
  X(0xc000013d, STATUS_REMOTE_RESOURCES,                        \
    ERROR_REM_NOT_LIST,                                         \
    WSAENOBUFS, ENOBUFS)                                        \
  X(0xc000013e, STATUS_LINK_FAILED,                             \
    ERROR_UNEXP_NET_ERR,                                        \
    WSAECONNRESET, ECONNRESET)                                  \
  X(0xc000013f, STATUS_LINK_TIMEOUT,                            \
    ERROR_UNEXP_NET_ERR,                                        \
    WSAETIMEDOUT, ETIMEDOUT)                                    \
  X(0xc0000140, STATUS_INVALID_CONNECTION,                      \
    ERROR_UNEXP_NET_ERR,                                        \
    WSAENOTCONN, ENOTCONN)                                      \
  X(0xc0000141, STATUS_INVALID_ADDRESS,                         \
    ERROR_UNEXP_NET_ERR,                                        \
    WSAEADDRNOTAVAIL, EADDRNOTAVAIL)                            \
  X(0xc0000142, STATUS_DLL_INIT_FAILED,                         \
    ERROR_DLL_INIT_FAILED,                                      \
    -1, -1)                                                     \
  X(0xc0000143, STATUS_MISSING_SYSTEMFILE,                      \
    ERROR_MISSING_SYSTEMFILE,                                   \
    -1, -1)                                                     \
  X(0xc0000144, STATUS_UNHANDLED_EXCEPTION,                     \
    ERROR_UNHANDLED_EXCEPTION,                                  \
    -1, -1)                                                     \
  X(0xc0000145, STATUS_APP_INIT_FAILURE,                        \
    ERROR_APP_INIT_FAILURE,                                     \
    -1, -1)                                                     \
  X(0xc0000146, STATUS_PAGEFILE_CREATE_FAILED,                  \
    ERROR_PAGEFILE_CREATE_FAILED,                               \
    -1, -1)                                                     \
  X(0xc0000147, STATUS_NO_PAGEFILE,                             \
    ERROR_NO_PAGEFILE,                                          \
    -1, -1)                                                     \
  X(0xc0000148, STATUS_INVALID_LEVEL,                           \
    ERROR_INVALID_LEVEL,                                        \
    -1, -1)                                                     \
  X(0xc0000149, STATUS_WRONG_PASSWORD_CORE,                     \
    ERROR_INVALID_PASSWORD,                                     \
    -1, -1)                                                     \
  X(0xc000014a, STATUS_ILLEGAL_FLOAT_CONTEXT,                   \
    ERROR_ILLEGAL_FLOAT_CONTEXT,                                \
    -1, -1)                                                     \
  X(0xc000014b, STATUS_PIPE_BROKEN,                             \
    ERROR_BROKEN_PIPE,                                          \
    -1, EPIPE)                                                  \
  X(0xc000014c, STATUS_REGISTRY_CORRUPT,                        \
    ERROR_BADDB,                                                \
    -1, -1)                                                     \
  X(0xc000014d, STATUS_REGISTRY_IO_FAILED,                      \
    ERROR_REGISTRY_IO_FAILED,                                   \
    -1, -1)                                                     \
  X(0xc000014e, STATUS_NO_EVENT_PAIR,                           \
    ERROR_NO_EVENT_PAIR,                                        \
    -1, -1)                                                     \
  X(0xc000014f, STATUS_UNRECOGNIZED_VOLUME,                     \
    ERROR_UNRECOGNIZED_VOLUME,                                  \
    -1, -1)                                                     \
  X(0xc0000150, STATUS_SERIAL_NO_DEVICE_INITED,                 \
    ERROR_SERIAL_NO_DEVICE,                                     \
    -1, -1)                                                     \
  X(0xc0000151, STATUS_NO_SUCH_ALIAS,                           \
    ERROR_NO_SUCH_ALIAS,                                        \
    -1, -1)                                                     \
  X(0xc0000152, STATUS_MEMBER_NOT_IN_ALIAS,                     \
    ERROR_MEMBER_NOT_IN_ALIAS,                                  \
    -1, -1)                                                     \
  X(0xc0000153, STATUS_MEMBER_IN_ALIAS,                         \
    ERROR_MEMBER_IN_ALIAS,                                      \
    -1, -1)                                                     \
  X(0xc0000154, STATUS_ALIAS_EXISTS,                            \
    ERROR_ALIAS_EXISTS,                                         \
    -1, -1)                                                     \
  X(0xc0000155, STATUS_LOGON_NOT_GRANTED,                       \
    ERROR_LOGON_NOT_GRANTED,                                    \
    -1, -1)                                                     \
  X(0xc0000156, STATUS_TOO_MANY_SECRETS,                        \
    ERROR_TOO_MANY_SECRETS,                                     \
    -1, -1)                                                     \
  X(0xc0000157, STATUS_SECRET_TOO_LONG,                         \
    ERROR_SECRET_TOO_LONG,                                      \
    -1, -1)                                                     \
  X(0xc0000158, STATUS_INTERNAL_DB_ERROR,                       \
    ERROR_INTERNAL_DB_ERROR,                                    \
    -1, -1)                                                     \
  X(0xc0000159, STATUS_FULLSCREEN_MODE,                         \
    ERROR_FULLSCREEN_MODE,                                      \
    -1, -1)                                                     \
  X(0xc000015a, STATUS_TOO_MANY_CONTEXT_IDS,                    \
    ERROR_TOO_MANY_CONTEXT_IDS,                                 \
    -1, -1)                                                     \
  X(0xc000015b, STATUS_LOGON_TYPE_NOT_GRANTED,                  \
    ERROR_LOGON_TYPE_NOT_GRANTED,                               \
    -1, -1)                                                     \
  X(0xc000015c, STATUS_NOT_REGISTRY_FILE,                       \
    ERROR_NOT_REGISTRY_FILE,                                    \
    -1, -1)                                                     \
  X(0xc000015d, STATUS_NT_CROSS_ENCRYPTION_REQUIRED,            \
    ERROR_NT_CROSS_ENCRYPTION_REQUIRED,                         \
    -1, -1)                                                     \
  X(0xc000015e, STATUS_DOMAIN_CTRLR_CONFIG_ERROR,               \
    ERROR_DOMAIN_CTRLR_CONFIG_ERROR,                            \
    -1, -1)                                                     \
  X(0xc000015f, STATUS_FT_MISSING_MEMBER,                       \
    ERROR_IO_DEVICE,                                            \
    -1, -1)                                                     \
  X(0xc0000160, STATUS_ILL_FORMED_SERVICE_ENTRY,                \
    ERROR_SERVICE_NOT_FOUND,                                    \
    -1, -1)                                                     \
  X(0xc0000161, STATUS_ILLEGAL_CHARACTER,                       \
    ERROR_ILLEGAL_CHARACTER,                                    \
    -1, -1)                                                     \
  X(0xc0000162, STATUS_UNMAPPABLE_CHARACTER,                    \
    ERROR_NO_UNICODE_TRANSLATION,                               \
    -1, -1)                                                     \
  X(0xc0000163, STATUS_UNDEFINED_CHARACTER,                     \
    ERROR_UNDEFINED_CHARACTER,                                  \
    -1, -1)                                                     \
  X(0xc0000164, STATUS_FLOPPY_VOLUME,                           \
    ERROR_FLOPPY_VOLUME,                                        \
    -1, -1)                                                     \
  X(0xc0000165, STATUS_FLOPPY_ID_MARK_NOT_FOUND,                \
    ERROR_FLOPPY_ID_MARK_NOT_FOUND,                             \
    -1, -1)                                                     \
  X(0xc0000166, STATUS_FLOPPY_WRONG_CYLINDER,                   \
    ERROR_FLOPPY_WRONG_CYLINDER,                                \
    -1, -1)                                                     \
  X(0xc0000167, STATUS_FLOPPY_UNKNOWN_ERROR,                    \
    ERROR_FLOPPY_UNKNOWN_ERROR,                                 \
    -1, -1)                                                     \
  X(0xc0000168, STATUS_FLOPPY_BAD_REGISTERS,                    \
    ERROR_FLOPPY_BAD_REGISTERS,                                 \
    -1, -1)                                                     \
  X(0xc0000169, STATUS_DISK_RECALIBRATE_FAILED,                 \
    ERROR_DISK_RECALIBRATE_FAILED,                              \
    -1, -1)                                                     \
  X(0xc000016a, STATUS_DISK_OPERATION_FAILED,                   \
    ERROR_DISK_OPERATION_FAILED,                                \
    -1, -1)                                                     \
  X(0xc000016b, STATUS_DISK_RESET_FAILED,                       \
    ERROR_DISK_RESET_FAILED,                                    \
    -1, -1)                                                     \
  X(0xc000016c, STATUS_SHARED_IRQ_BUSY,                         \
    ERROR_IRQ_BUSY,                                             \
    -1, -1)                                                     \
  X(0xc000016d, STATUS_FT_ORPHANING,                            \
    ERROR_IO_DEVICE,                                            \
    -1, -1)                                                     \
  X(0xc000016e, STATUS_BIOS_FAILED_TO_CONNECT_INTERRUPT,        \
    ERROR_BIOS_FAILED_TO_CONNECT_INTERRUPT,                     \
    -1, -1)                                                     \
  X(0xc0000172, STATUS_PARTITION_FAILURE,                       \
    ERROR_PARTITION_FAILURE,                                    \
    -1, -1)                                                     \
  X(0xc0000173, STATUS_INVALID_BLOCK_LENGTH,                    \
    ERROR_INVALID_BLOCK_LENGTH,                                 \
    -1, -1)                                                     \
  X(0xc0000174, STATUS_DEVICE_NOT_PARTITIONED,                  \
    ERROR_DEVICE_NOT_PARTITIONED,                               \
    -1, -1)                                                     \
  X(0xc0000175, STATUS_UNABLE_TO_LOCK_MEDIA,                    \
    ERROR_UNABLE_TO_LOCK_MEDIA,                                 \
    -1, -1)                                                     \
  X(0xc0000176, STATUS_UNABLE_TO_UNLOAD_MEDIA,                  \
    ERROR_UNABLE_TO_UNLOAD_MEDIA,                               \
    -1, -1)                                                     \
  X(0xc0000177, STATUS_EOM_OVERFLOW,                            \
    ERROR_EOM_OVERFLOW,                                         \
    -1, -1)                                                     \
  X(0xc0000178, STATUS_NO_MEDIA,                                \
    ERROR_NO_MEDIA_IN_DRIVE,                                    \
    -1, -1)                                                     \
  X(0xc000017a, STATUS_NO_SUCH_MEMBER,                          \
    ERROR_NO_SUCH_MEMBER,                                       \
    -1, -1)                                                     \
  X(0xc000017b, STATUS_INVALID_MEMBER,                          \
    ERROR_INVALID_MEMBER,                                       \
    -1, -1)                                                     \
  X(0xc000017c, STATUS_KEY_DELETED,                             \
    ERROR_KEY_DELETED,                                          \
    -1, -1)                                                     \
  X(0xc000017d, STATUS_NO_LOG_SPACE,                            \
    ERROR_NO_LOG_SPACE,                                         \
    -1, -1)                                                     \
  X(0xc000017e, STATUS_TOO_MANY_SIDS,                           \
    ERROR_TOO_MANY_SIDS,                                        \
    -1, -1)                                                     \
  X(0xc000017f, STATUS_LM_CROSS_ENCRYPTION_REQUIRED,            \
    ERROR_LM_CROSS_ENCRYPTION_REQUIRED,                         \
    -1, -1)                                                     \
  X(0xc0000180, STATUS_KEY_HAS_CHILDREN,                        \
    ERROR_KEY_HAS_CHILDREN,                                     \
    -1, -1)                                                     \
  X(0xc0000181, STATUS_CHILD_MUST_BE_VOLATILE,                  \
    ERROR_CHILD_MUST_BE_VOLATILE,                               \
    -1, -1)                                                     \
  X(0xc0000182, STATUS_DEVICE_CONFIGURATION_ERROR,              \
    ERROR_INVALID_PARAMETER,                                    \
    -1, EINVAL)                                                 \
  X(0xc0000183, STATUS_DRIVER_INTERNAL_ERROR,                   \
    ERROR_IO_DEVICE,                                            \
    -1, -1)                                                     \
  X(0xc0000184, STATUS_INVALID_DEVICE_STATE,                    \
    ERROR_BAD_COMMAND,                                          \
    -1, EACCES)                                                 \
  X(0xc0000185, STATUS_IO_DEVICE_ERROR,                         \
    ERROR_IO_DEVICE,                                            \
    -1, -1)                                                     \
  X(0xc0000186, STATUS_DEVICE_PROTOCOL_ERROR,                   \
    ERROR_IO_DEVICE,                                            \
    -1, -1)                                                     \
  X(0xc0000187, STATUS_BACKUP_CONTROLLER,                       \
    ERROR_BACKUP_CONTROLLER,                                    \
    -1, -1)                                                     \
  X(0xc0000188, STATUS_LOG_FILE_FULL,                           \
    ERROR_LOG_FILE_FULL,                                        \
    -1, -1)                                                     \
  X(0xc0000189, STATUS_TOO_LATE,                                \
    ERROR_WRITE_PROTECT,                                        \
    -1, EACCES)                                                 \
  X(0xc000018a, STATUS_NO_TRUST_LSA_SECRET,                     \
    ERROR_NO_TRUST_LSA_SECRET,                                  \
    -1, -1)                                                     \
  X(0xc000018b, STATUS_NO_TRUST_SAM_ACCOUNT,                    \
    ERROR_NO_TRUST_SAM_ACCOUNT,                                 \
    -1, -1)                                                     \
  X(0xc000018c, STATUS_TRUSTED_DOMAIN_FAILURE,                  \
    ERROR_TRUSTED_DOMAIN_FAILURE,                               \
    -1, -1)                                                     \
  X(0xc000018d, STATUS_TRUSTED_RELATIONSHIP_FAILURE,            \
    ERROR_TRUSTED_RELATIONSHIP_FAILURE,                         \
    -1, -1)                                                     \
  X(0xc000018e, STATUS_EVENTLOG_FILE_CORRUPT,                   \
    ERROR_EVENTLOG_FILE_CORRUPT,                                \
    -1, -1)                                                     \
  X(0xc000018f, STATUS_EVENTLOG_CANT_START,                     \
    ERROR_EVENTLOG_CANT_START,                                  \
    -1, -1)                                                     \
  X(0xc0000190, STATUS_TRUST_FAILURE,                           \
    ERROR_TRUST_FAILURE,                                        \
    -1, -1)                                                     \
  X(0xc0000191, STATUS_MUTANT_LIMIT_EXCEEDED,                   \
    ERROR_MUTANT_LIMIT_EXCEEDED,                                \
    -1, -1)                                                     \
  X(0xc0000192, STATUS_NETLOGON_NOT_STARTED,                    \
    ERROR_NETLOGON_NOT_STARTED,                                 \
    -1, -1)                                                     \
  X(0xc0000193, STATUS_ACCOUNT_EXPIRED,                         \
    ERROR_ACCOUNT_EXPIRED,                                      \
    -1, -1)                                                     \
  X(0xc0000194, STATUS_POSSIBLE_DEADLOCK,                       \
    ERROR_POSSIBLE_DEADLOCK,                                    \
    -1, -1)                                                     \
  X(0xc0000195, STATUS_NETWORK_CREDENTIAL_CONFLICT,             \
    ERROR_SESSION_CREDENTIAL_CONFLICT,                          \
    -1, -1)                                                     \
  X(0xc0000196, STATUS_REMOTE_SESSION_LIMIT,                    \
    ERROR_REMOTE_SESSION_LIMIT_EXCEEDED,                        \
    -1, -1)                                                     \
  X(0xc0000197, STATUS_EVENTLOG_FILE_CHANGED,                   \
    ERROR_EVENTLOG_FILE_CHANGED,                                \
    -1, -1)                                                     \
  X(0xc0000198, STATUS_NOLOGON_INTERDOMAIN_TRUST_ACCOUNT,       \
    ERROR_NOLOGON_INTERDOMAIN_TRUST_ACCOUNT,                    \
    -1, -1)                                                     \
  X(0xc0000199, STATUS_NOLOGON_WORKSTATION_TRUST_ACCOUNT,       \
    ERROR_NOLOGON_WORKSTATION_TRUST_ACCOUNT,                    \
    -1, -1)                                                     \
  X(0xc000019a, STATUS_NOLOGON_SERVER_TRUST_ACCOUNT,            \
    ERROR_NOLOGON_SERVER_TRUST_ACCOUNT,                         \
    -1, -1)                                                     \
  X(0xc000019b, STATUS_DOMAIN_TRUST_INCONSISTENT,               \
    ERROR_DOMAIN_TRUST_INCONSISTENT,                            \
    -1, -1)                                                     \
  X(0xc000019c, STATUS_FS_DRIVER_REQUIRED,                      \
    ERROR_FS_DRIVER_REQUIRED,                                   \
    -1, -1)                                                     \
  X(0xc000019d, STATUS_IMAGE_ALREADY_LOADED_AS_DLL,             \
    ERROR_SERVICE_ALREADY_RUNNING,                              \
    -1, -1)                                                     \
  X(0xc000019e, /* ntstatus symbol not available */,            \
    ERROR_INCOMPATIBLE_WITH_GLOBAL_SHORT_NAME_REGISTRY_SETTING, \
    -1, -1)                                                     \
  X(0xc000019f, STATUS_SHORT_NAMES_NOT_ENABLED_ON_VOLUME,       \
    ERROR_SHORT_NAMES_NOT_ENABLED_ON_VOLUME,                    \
    -1, -1)                                                     \
  X(0xc00001a0, STATUS_SECURITY_STREAM_IS_INCONSISTENT,         \
    ERROR_SECURITY_STREAM_IS_INCONSISTENT,                      \
    -1, -1)                                                     \
  X(0xc00001a1, STATUS_INVALID_LOCK_RANGE,                      \
    ERROR_INVALID_LOCK_RANGE,                                   \
    -1, -1)                                                     \
  X(0xc00001a2, STATUS_INVALID_ACE_CONDITION,                   \
    ERROR_INVALID_ACE_CONDITION,                                \
    -1, -1)                                                     \
  X(0xc00001a3, STATUS_IMAGE_SUBSYSTEM_NOT_PRESENT,             \
    ERROR_IMAGE_SUBSYSTEM_NOT_PRESENT,                          \
    -1, -1)                                                     \
  X(0xc00001a4, STATUS_NOTIFICATION_GUID_ALREADY_DEFINED,       \
    ERROR_NOTIFICATION_GUID_ALREADY_DEFINED,                    \
    -1, -1)                                                     \
  X(0xc00001a8, /* ntstatus symbol not available */,            \
    ERROR_DATA_LOST_REPAIR,                                     \
    -1, -1)                                                     \
  X(0xc00001a9, /* ntstatus symbol not available */,            \
    ERROR_NOT_SUPPORTED,                                        \
    -1, -1)                                                     \
  X(0xc00001ad, /* ntstatus symbol not available */,            \
    ERROR_NOT_ENOUGH_MEMORY,                                    \
    -1, ENOMEM)                                                 \
  X(0xc00001ae, /* ntstatus symbol not available */,            \
    ERROR_PROCESS_NOT_IN_JOB,                                   \
    -1, -1)                                                     \
  X(0xc0000201, STATUS_NETWORK_OPEN_RESTRICTION,                \
    ERROR_NETWORK_ACCESS_DENIED,                                \
    -1, EACCES)                                                 \
  X(0xc0000202, STATUS_NO_USER_SESSION_KEY,                     \
    ERROR_NO_USER_SESSION_KEY,                                  \
    -1, -1)                                                     \
  X(0xc0000203, STATUS_USER_SESSION_DELETED,                    \
    ERROR_UNEXP_NET_ERR,                                        \
    -1, -1)                                                     \
  X(0xc0000204, STATUS_RESOURCE_LANG_NOT_FOUND,                 \
    ERROR_RESOURCE_LANG_NOT_FOUND,                              \
    -1, -1)                                                     \
  X(0xc0000205, STATUS_INSUFF_SERVER_RESOURCES,                 \
    ERROR_NOT_ENOUGH_SERVER_MEMORY,                             \
    -1, -1)                                                     \
  X(0xc0000206, STATUS_INVALID_BUFFER_SIZE,                     \
    ERROR_INVALID_USER_BUFFER,                                  \
    WSAEMSGSIZE, EMSGSIZE)                                      \
  X(0xc0000207, STATUS_INVALID_ADDRESS_COMPONENT,               \
    ERROR_INVALID_NETNAME,                                      \
    WSAEADDRNOTAVAIL, EADDRNOTAVAIL)                            \
  X(0xc0000208, STATUS_INVALID_ADDRESS_WILDCARD,                \
    ERROR_INVALID_NETNAME,                                      \
    -1, -1)                                                     \
  X(0xc0000209, STATUS_TOO_MANY_ADDRESSES,                      \
    ERROR_TOO_MANY_NAMES,                                       \
    WSAEADDRINUSE, EADDRINUSE)                                  \
  X(0xc000020a, STATUS_ADDRESS_ALREADY_EXISTS,                  \
    ERROR_DUP_NAME,                                             \
    WSAEADDRINUSE, EADDRINUSE)                                  \
  X(0xc000020b, STATUS_ADDRESS_CLOSED,                          \
    ERROR_NETNAME_DELETED,                                      \
    -1, -1)                                                     \
  X(0xc000020c, STATUS_CONNECTION_DISCONNECTED,                 \
    ERROR_NETNAME_DELETED,                                      \
    WSAECONNRESET, ECONNRESET)                                  \
  X(0xc000020d, STATUS_CONNECTION_RESET,                        \
    ERROR_NETNAME_DELETED,                                      \
    WSAECONNRESET, ECONNRESET)                                  \
  X(0xc000020e, STATUS_TOO_MANY_NODES,                          \
    ERROR_TOO_MANY_NAMES,                                       \
    -1, -1)                                                     \
  X(0xc000020f, STATUS_TRANSACTION_ABORTED,                     \
    ERROR_UNEXP_NET_ERR,                                        \
    WSAECONNABORTED, ECONNABORTED)                              \
  X(0xc0000210, STATUS_TRANSACTION_TIMED_OUT,                   \
    ERROR_UNEXP_NET_ERR,                                        \
    -1, -1)                                                     \
  X(0xc0000211, STATUS_TRANSACTION_NO_RELEASE,                  \
    ERROR_UNEXP_NET_ERR,                                        \
    -1, -1)                                                     \
  X(0xc0000212, STATUS_TRANSACTION_NO_MATCH,                    \
    ERROR_UNEXP_NET_ERR,                                        \
    -1, -1)                                                     \
  X(0xc0000213, STATUS_TRANSACTION_RESPONDED,                   \
    ERROR_UNEXP_NET_ERR,                                        \
    -1, -1)                                                     \
  X(0xc0000214, STATUS_TRANSACTION_INVALID_ID,                  \
    ERROR_UNEXP_NET_ERR,                                        \
    -1, -1)                                                     \
  X(0xc0000215, STATUS_TRANSACTION_INVALID_TYPE,                \
    ERROR_UNEXP_NET_ERR,                                        \
    -1, -1)                                                     \
  X(0xc0000216, STATUS_NOT_SERVER_SESSION,                      \
    ERROR_NOT_SUPPORTED,                                        \
    -1, -1)                                                     \
  X(0xc0000217, STATUS_NOT_CLIENT_SESSION,                      \
    ERROR_NOT_SUPPORTED,                                        \
    -1, -1)                                                     \
  X(0xc0000218, STATUS_CANNOT_LOAD_REGISTRY_FILE,               \
    ERROR_CANNOT_LOAD_REGISTRY_FILE,                            \
    -1, -1)                                                     \
  X(0xc0000219, STATUS_DEBUG_ATTACH_FAILED,                     \
    ERROR_DEBUG_ATTACH_FAILED,                                  \
    -1, -1)                                                     \
  X(0xc000021a, STATUS_SYSTEM_PROCESS_TERMINATED,               \
    ERROR_SYSTEM_PROCESS_TERMINATED,                            \
    -1, -1)                                                     \
  X(0xc000021b, STATUS_DATA_NOT_ACCEPTED,                       \
    ERROR_DATA_NOT_ACCEPTED,                                    \
    -1, -1)                                                     \
  X(0xc000021c, STATUS_NO_BROWSER_SERVERS_FOUND,                \
    ERROR_NO_BROWSER_SERVERS_FOUND,                             \
    -1, -1)                                                     \
  X(0xc000021d, STATUS_VDM_HARD_ERROR,                          \
    ERROR_VDM_HARD_ERROR,                                       \
    -1, -1)                                                     \
  X(0xc000021e, STATUS_DRIVER_CANCEL_TIMEOUT,                   \
    ERROR_DRIVER_CANCEL_TIMEOUT,                                \
    -1, -1)                                                     \
  X(0xc000021f, STATUS_REPLY_MESSAGE_MISMATCH,                  \
    ERROR_REPLY_MESSAGE_MISMATCH,                               \
    -1, -1)                                                     \
  X(0xc0000220, STATUS_MAPPED_ALIGNMENT,                        \
    ERROR_MAPPED_ALIGNMENT,                                     \
    -1, -1)                                                     \
  X(0xc0000221, STATUS_IMAGE_CHECKSUM_MISMATCH,                 \
    ERROR_BAD_EXE_FORMAT,                                       \
    -1, ENOEXEC)                                                \
  X(0xc0000222, STATUS_LOST_WRITEBEHIND_DATA,                   \
    ERROR_LOST_WRITEBEHIND_DATA,                                \
    -1, -1)                                                     \
  X(0xc0000223, STATUS_CLIENT_SERVER_PARAMETERS_INVALID,        \
    ERROR_CLIENT_SERVER_PARAMETERS_INVALID,                     \
    -1, -1)                                                     \
  X(0xc0000224, STATUS_PASSWORD_MUST_CHANGE,                    \
    ERROR_PASSWORD_MUST_CHANGE,                                 \
    -1, -1)                                                     \
  X(0xc0000225, STATUS_NOT_FOUND,                               \
    ERROR_NOT_FOUND,                                            \
    -1, -1)                                                     \
  X(0xc0000226, STATUS_NOT_TINY_STREAM,                         \
    ERROR_NOT_TINY_STREAM,                                      \
    -1, -1)                                                     \
  X(0xc0000227, STATUS_RECOVERY_FAILURE,                        \
    ERROR_RECOVERY_FAILURE,                                     \
    -1, -1)                                                     \
  X(0xc0000228, STATUS_STACK_OVERFLOW_READ,                     \
    ERROR_STACK_OVERFLOW_READ,                                  \
    -1, -1)                                                     \
  X(0xc0000229, STATUS_FAIL_CHECK,                              \
    ERROR_INVALID_PARAMETER,                                    \
    -1, EINVAL)                                                 \
  X(0xc000022a, STATUS_DUPLICATE_OBJECTID,                      \
    ERROR_OBJECT_ALREADY_EXISTS,                                \
    -1, -1)                                                     \
  X(0xc000022b, STATUS_OBJECTID_EXISTS,                         \
    ERROR_OBJECT_ALREADY_EXISTS,                                \
    -1, -1)                                                     \
  X(0xc000022c, STATUS_CONVERT_TO_LARGE,                        \
    ERROR_CONVERT_TO_LARGE,                                     \
    -1, -1)                                                     \
  X(0xc000022d, STATUS_RETRY,                                   \
    ERROR_RETRY,                                                \
    -1, -1)                                                     \
  X(0xc000022e, STATUS_FOUND_OUT_OF_SCOPE,                      \
    ERROR_FOUND_OUT_OF_SCOPE,                                   \
    -1, -1)                                                     \
  X(0xc000022f, STATUS_ALLOCATE_BUCKET,                         \
    ERROR_ALLOCATE_BUCKET,                                      \
    -1, -1)                                                     \
  X(0xc0000230, STATUS_PROPSET_NOT_FOUND,                       \
    ERROR_SET_NOT_FOUND,                                        \
    -1, -1)                                                     \
  X(0xc0000231, STATUS_MARSHALL_OVERFLOW,                       \
    ERROR_MARSHALL_OVERFLOW,                                    \
    -1, -1)                                                     \
  X(0xc0000232, STATUS_INVALID_VARIANT,                         \
    ERROR_INVALID_VARIANT,                                      \
    -1, -1)                                                     \
  X(0xc0000233, STATUS_DOMAIN_CONTROLLER_NOT_FOUND,             \
    ERROR_DOMAIN_CONTROLLER_NOT_FOUND,                          \
    -1, -1)                                                     \
  X(0xc0000234, STATUS_ACCOUNT_LOCKED_OUT,                      \
    ERROR_ACCOUNT_LOCKED_OUT,                                   \
    -1, -1)                                                     \
  X(0xc0000235, STATUS_HANDLE_NOT_CLOSABLE,                     \
    ERROR_INVALID_HANDLE,                                       \
    -1, EBADF)                                                  \
  X(0xc0000236, STATUS_CONNECTION_REFUSED,                      \
    ERROR_CONNECTION_REFUSED,                                   \
    WSAECONNREFUSED, ECONNREFUSED)                              \
  X(0xc0000237, STATUS_GRACEFUL_DISCONNECT,                     \
    ERROR_GRACEFUL_DISCONNECT,                                  \
    WSAEDISCON, EPIPE)                                          \
  X(0xc0000238, STATUS_ADDRESS_ALREADY_ASSOCIATED,              \
    ERROR_ADDRESS_ALREADY_ASSOCIATED,                           \
    -1, -1)                                                     \
  X(0xc0000239, STATUS_ADDRESS_NOT_ASSOCIATED,                  \
    ERROR_ADDRESS_NOT_ASSOCIATED,                               \
    -1, -1)                                                     \
  X(0xc000023a, STATUS_CONNECTION_INVALID,                      \
    ERROR_CONNECTION_INVALID,                                   \
    -1, -1)                                                     \
  X(0xc000023b, STATUS_CONNECTION_ACTIVE,                       \
    ERROR_CONNECTION_ACTIVE,                                    \
    -1, -1)                                                     \
  X(0xc000023c, STATUS_NETWORK_UNREACHABLE,                     \
    ERROR_NETWORK_UNREACHABLE,                                  \
    WSAENETUNREACH, ENETUNREACH)                                \
  X(0xc000023d, STATUS_HOST_UNREACHABLE,                        \
    ERROR_HOST_UNREACHABLE,                                     \
    WSAEHOSTUNREACH, EHOSTUNREACH)                              \
  X(0xc000023e, STATUS_PROTOCOL_UNREACHABLE,                    \
    ERROR_PROTOCOL_UNREACHABLE,                                 \
    WSAENETUNREACH, ENETUNREACH)                                \
  X(0xc000023f, STATUS_PORT_UNREACHABLE,                        \
    ERROR_PORT_UNREACHABLE,                                     \
    WSAECONNRESET, ECONNRESET)                                  \
  X(0xc0000240, STATUS_REQUEST_ABORTED,                         \
    ERROR_REQUEST_ABORTED,                                      \
    WSAEINTR, EINTR)                                            \
  X(0xc0000241, STATUS_CONNECTION_ABORTED,                      \
    ERROR_CONNECTION_ABORTED,                                   \
    WSAECONNABORTED, ECONNABORTED)                              \
  X(0xc0000242, STATUS_BAD_COMPRESSION_BUFFER,                  \
    ERROR_BAD_COMPRESSION_BUFFER,                               \
    -1, -1)                                                     \
  X(0xc0000243, STATUS_USER_MAPPED_FILE,                        \
    ERROR_USER_MAPPED_FILE,                                     \
    -1, -1)                                                     \
  X(0xc0000244, STATUS_AUDIT_FAILED,                            \
    ERROR_AUDIT_FAILED,                                         \
    -1, -1)                                                     \
  X(0xc0000245, STATUS_TIMER_RESOLUTION_NOT_SET,                \
    ERROR_TIMER_RESOLUTION_NOT_SET,                             \
    -1, -1)                                                     \
  X(0xc0000246, STATUS_CONNECTION_COUNT_LIMIT,                  \
    ERROR_CONNECTION_COUNT_LIMIT,                               \
    -1, -1)                                                     \
  X(0xc0000247, STATUS_LOGIN_TIME_RESTRICTION,                  \
    ERROR_LOGIN_TIME_RESTRICTION,                               \
    -1, -1)                                                     \
  X(0xc0000248, STATUS_LOGIN_WKSTA_RESTRICTION,                 \
    ERROR_LOGIN_WKSTA_RESTRICTION,                              \
    -1, -1)                                                     \
  X(0xc0000249, STATUS_IMAGE_MP_UP_MISMATCH,                    \
    ERROR_BAD_EXE_FORMAT,                                       \
    -1, ENOEXEC)                                                \
  X(0xc0000250, STATUS_INSUFFICIENT_LOGON_INFO,                 \
    ERROR_INSUFFICIENT_LOGON_INFO,                              \
    -1, -1)                                                     \
  X(0xc0000251, STATUS_BAD_DLL_ENTRYPOINT,                      \
    ERROR_BAD_DLL_ENTRYPOINT,                                   \
    -1, -1)                                                     \
  X(0xc0000252, STATUS_BAD_SERVICE_ENTRYPOINT,                  \
    ERROR_BAD_SERVICE_ENTRYPOINT,                               \
    -1, -1)                                                     \
  X(0xc0000253, STATUS_LPC_REPLY_LOST,                          \
    ERROR_CONNECTION_ABORTED,                                   \
    -1, -1)                                                     \
  X(0xc0000254, STATUS_IP_ADDRESS_CONFLICT1,                    \
    ERROR_IP_ADDRESS_CONFLICT1,                                 \
    -1, -1)                                                     \
  X(0xc0000255, STATUS_IP_ADDRESS_CONFLICT2,                    \
    ERROR_IP_ADDRESS_CONFLICT2,                                 \
    -1, -1)                                                     \
  X(0xc0000256, STATUS_REGISTRY_QUOTA_LIMIT,                    \
    ERROR_REGISTRY_QUOTA_LIMIT,                                 \
    -1, -1)                                                     \
  X(0xc0000257, STATUS_PATH_NOT_COVERED,                        \
    ERROR_HOST_UNREACHABLE,                                     \
    -1, -1)                                                     \
  X(0xc0000258, STATUS_NO_CALLBACK_ACTIVE,                      \
    ERROR_NO_CALLBACK_ACTIVE,                                   \
    -1, -1)                                                     \
  X(0xc0000259, STATUS_LICENSE_QUOTA_EXCEEDED,                  \
    ERROR_LICENSE_QUOTA_EXCEEDED,                               \
    -1, -1)                                                     \
  X(0xc000025a, STATUS_PWD_TOO_SHORT,                           \
    ERROR_PWD_TOO_SHORT,                                        \
    -1, -1)                                                     \
  X(0xc000025b, STATUS_PWD_TOO_RECENT,                          \
    ERROR_PWD_TOO_RECENT,                                       \
    -1, -1)                                                     \
  X(0xc000025c, STATUS_PWD_HISTORY_CONFLICT,                    \
    ERROR_PWD_HISTORY_CONFLICT,                                 \
    -1, -1)                                                     \
  X(0xc000025e, STATUS_PLUGPLAY_NO_DEVICE,                      \
    ERROR_SERVICE_DISABLED,                                     \
    -1, -1)                                                     \
  X(0xc000025f, STATUS_UNSUPPORTED_COMPRESSION,                 \
    ERROR_UNSUPPORTED_COMPRESSION,                              \
    -1, -1)                                                     \
  X(0xc0000260, STATUS_INVALID_HW_PROFILE,                      \
    ERROR_INVALID_HW_PROFILE,                                   \
    -1, -1)                                                     \
  X(0xc0000261, STATUS_INVALID_PLUGPLAY_DEVICE_PATH,            \
    ERROR_INVALID_PLUGPLAY_DEVICE_PATH,                         \
    -1, -1)                                                     \
  X(0xc0000262, STATUS_DRIVER_ORDINAL_NOT_FOUND,                \
    ERROR_INVALID_ORDINAL,                                      \
    -1, -1)                                                     \
  X(0xc0000263, STATUS_DRIVER_ENTRYPOINT_NOT_FOUND,             \
    ERROR_PROC_NOT_FOUND,                                       \
    -1, -1)                                                     \
  X(0xc0000264, STATUS_RESOURCE_NOT_OWNED,                      \
    ERROR_NOT_OWNER,                                            \
    -1, -1)                                                     \
  X(0xc0000265, STATUS_TOO_MANY_LINKS,                          \
    ERROR_TOO_MANY_LINKS,                                       \
    -1, -1)                                                     \
  X(0xc0000266, STATUS_QUOTA_LIST_INCONSISTENT,                 \
    ERROR_QUOTA_LIST_INCONSISTENT,                              \
    -1, -1)                                                     \
  X(0xc0000267, STATUS_FILE_IS_OFFLINE,                         \
    ERROR_FILE_OFFLINE,                                         \
    -1, -1)                                                     \
  X(0xc0000268, STATUS_EVALUATION_EXPIRATION,                   \
    ERROR_EVALUATION_EXPIRATION,                                \
    -1, -1)                                                     \
  X(0xc0000269, STATUS_ILLEGAL_DLL_RELOCATION,                  \
    ERROR_ILLEGAL_DLL_RELOCATION,                               \
    -1, -1)                                                     \
  X(0xc000026a, STATUS_LICENSE_VIOLATION,                       \
    ERROR_CTX_LICENSE_NOT_AVAILABLE,                            \
    -1, -1)                                                     \
  X(0xc000026b, STATUS_DLL_INIT_FAILED_LOGOFF,                  \
    ERROR_DLL_INIT_FAILED_LOGOFF,                               \
    -1, -1)                                                     \
  X(0xc000026c, STATUS_DRIVER_UNABLE_TO_LOAD,                   \
    ERROR_BAD_DRIVER,                                           \
    -1, -1)                                                     \
  X(0xc000026d, STATUS_DFS_UNAVAILABLE,                         \
    ERROR_CONNECTION_UNAVAIL,                                   \
    -1, -1)                                                     \
  X(0xc000026e, STATUS_VOLUME_DISMOUNTED,                       \
    ERROR_NOT_READY,                                            \
    -1, EACCES)                                                 \
  X(0xc000026f, STATUS_WX86_INTERNAL_ERROR,                     \
    ERROR_WX86_ERROR,                                           \
    -1, -1)                                                     \
  X(0xc0000270, STATUS_WX86_FLOAT_STACK_CHECK,                  \
    ERROR_WX86_ERROR,                                           \
    -1, -1)                                                     \
  X(0xc0000271, STATUS_VALIDATE_CONTINUE,                       \
    ERROR_VALIDATE_CONTINUE,                                    \
    -1, -1)                                                     \
  X(0xc0000272, STATUS_NO_MATCH,                                \
    ERROR_NO_MATCH,                                             \
    -1, -1)                                                     \
  X(0xc0000273, STATUS_NO_MORE_MATCHES,                         \
    ERROR_NO_MORE_MATCHES,                                      \
    -1, -1)                                                     \
  X(0xc0000275, STATUS_NOT_A_REPARSE_POINT,                     \
    ERROR_NOT_A_REPARSE_POINT,                                  \
    -1, -1)                                                     \
  X(0xc0000276, STATUS_IO_REPARSE_TAG_INVALID,                  \
    ERROR_REPARSE_TAG_INVALID,                                  \
    -1, -1)                                                     \
  X(0xc0000277, STATUS_IO_REPARSE_TAG_MISMATCH,                 \
    ERROR_REPARSE_TAG_MISMATCH,                                 \
    -1, -1)                                                     \
  X(0xc0000278, STATUS_IO_REPARSE_DATA_INVALID,                 \
    ERROR_INVALID_REPARSE_DATA,                                 \
    -1, -1)                                                     \
  X(0xc0000279, STATUS_IO_REPARSE_TAG_NOT_HANDLED,              \
    ERROR_CANT_ACCESS_FILE,                                     \
    -1, -1)                                                     \
  X(0xc000027b, /* ntstatus symbol not available */,            \
    ERROR_INTERNAL_ERROR,                                       \
    -1, -1)                                                     \
  X(0xc0000280, STATUS_REPARSE_POINT_NOT_RESOLVED,              \
    ERROR_CANT_RESOLVE_FILENAME,                                \
    -1, -1)                                                     \
  X(0xc0000281, STATUS_DIRECTORY_IS_A_REPARSE_POINT,            \
    ERROR_BAD_PATHNAME,                                         \
    -1, ENOENT)                                                 \
  X(0xc0000282, STATUS_RANGE_LIST_CONFLICT,                     \
    ERROR_RANGE_LIST_CONFLICT,                                  \
    -1, -1)                                                     \
  X(0xc0000283, STATUS_SOURCE_ELEMENT_EMPTY,                    \
    ERROR_SOURCE_ELEMENT_EMPTY,                                 \
    -1, -1)                                                     \
  X(0xc0000284, STATUS_DESTINATION_ELEMENT_FULL,                \
    ERROR_DESTINATION_ELEMENT_FULL,                             \
    -1, -1)                                                     \
  X(0xc0000285, STATUS_ILLEGAL_ELEMENT_ADDRESS,                 \
    ERROR_ILLEGAL_ELEMENT_ADDRESS,                              \
    -1, -1)                                                     \
  X(0xc0000286, STATUS_MAGAZINE_NOT_PRESENT,                    \
    ERROR_MAGAZINE_NOT_PRESENT,                                 \
    -1, -1)                                                     \
  X(0xc0000287, STATUS_REINITIALIZATION_NEEDED,                 \
    ERROR_DEVICE_REINITIALIZATION_NEEDED,                       \
    -1, -1)                                                     \
  X(0xc000028a, STATUS_ENCRYPTION_FAILED,                       \
    ERROR_ACCESS_DENIED,                                        \
    -1, EACCES)                                                 \
  X(0xc000028b, STATUS_DECRYPTION_FAILED,                       \
    ERROR_ACCESS_DENIED,                                        \
    -1, EACCES)                                                 \
  X(0xc000028c, STATUS_RANGE_NOT_FOUND,                         \
    ERROR_RANGE_NOT_FOUND,                                      \
    -1, -1)                                                     \
  X(0xc000028d, STATUS_NO_RECOVERY_POLICY,                      \
    ERROR_ACCESS_DENIED,                                        \
    -1, EACCES)                                                 \
  X(0xc000028e, STATUS_NO_EFS,                                  \
    ERROR_ACCESS_DENIED,                                        \
    -1, EACCES)                                                 \
  X(0xc000028f, STATUS_WRONG_EFS,                               \
    ERROR_ACCESS_DENIED,                                        \
    -1, EACCES)                                                 \
  X(0xc0000290, STATUS_NO_USER_KEYS,                            \
    ERROR_ACCESS_DENIED,                                        \
    -1, EACCES)                                                 \
  X(0xc0000291, STATUS_FILE_NOT_ENCRYPTED,                      \
    ERROR_FILE_NOT_ENCRYPTED,                                   \
    -1, -1)                                                     \
  X(0xc0000292, STATUS_NOT_EXPORT_FORMAT,                       \
    ERROR_NOT_EXPORT_FORMAT,                                    \
    -1, -1)                                                     \
  X(0xc0000293, STATUS_FILE_ENCRYPTED,                          \
    ERROR_FILE_ENCRYPTED,                                       \
    -1, -1)                                                     \
  X(0xc0000295, STATUS_WMI_GUID_NOT_FOUND,                      \
    ERROR_WMI_GUID_NOT_FOUND,                                   \
    -1, -1)                                                     \
  X(0xc0000296, STATUS_WMI_INSTANCE_NOT_FOUND,                  \
    ERROR_WMI_INSTANCE_NOT_FOUND,                               \
    -1, -1)                                                     \
  X(0xc0000297, STATUS_WMI_ITEMID_NOT_FOUND,                    \
    ERROR_WMI_ITEMID_NOT_FOUND,                                 \
    -1, -1)                                                     \
  X(0xc0000298, STATUS_WMI_TRY_AGAIN,                           \
    ERROR_WMI_TRY_AGAIN,                                        \
    -1, -1)                                                     \
  X(0xc0000299, STATUS_SHARED_POLICY,                           \
    ERROR_SHARED_POLICY,                                        \
    -1, -1)                                                     \
  X(0xc000029a, STATUS_POLICY_OBJECT_NOT_FOUND,                 \
    ERROR_POLICY_OBJECT_NOT_FOUND,                              \
    -1, -1)                                                     \
  X(0xc000029b, STATUS_POLICY_ONLY_IN_DS,                       \
    ERROR_POLICY_ONLY_IN_DS,                                    \
    -1, -1)                                                     \
  X(0xc000029c, STATUS_VOLUME_NOT_UPGRADED,                     \
    ERROR_INVALID_FUNCTION,                                     \
    -1, EINVAL)                                                 \
  X(0xc000029d, STATUS_REMOTE_STORAGE_NOT_ACTIVE,               \
    ERROR_REMOTE_STORAGE_NOT_ACTIVE,                            \
    -1, -1)                                                     \
  X(0xc000029e, STATUS_REMOTE_STORAGE_MEDIA_ERROR,              \
    ERROR_REMOTE_STORAGE_MEDIA_ERROR,                           \
    -1, -1)                                                     \
  X(0xc000029f, STATUS_NO_TRACKING_SERVICE,                     \
    ERROR_NO_TRACKING_SERVICE,                                  \
    -1, -1)                                                     \
  X(0xc00002a0, STATUS_SERVER_SID_MISMATCH,                     \
    ERROR_SERVER_SID_MISMATCH,                                  \
    -1, -1)                                                     \
  X(0xc00002a1, STATUS_DS_NO_ATTRIBUTE_OR_VALUE,                \
    ERROR_DS_NO_ATTRIBUTE_OR_VALUE,                             \
    -1, -1)                                                     \
  X(0xc00002a2, STATUS_DS_INVALID_ATTRIBUTE_SYNTAX,             \
    ERROR_DS_INVALID_ATTRIBUTE_SYNTAX,                          \
    -1, -1)                                                     \
  X(0xc00002a3, STATUS_DS_ATTRIBUTE_TYPE_UNDEFINED,             \
    ERROR_DS_ATTRIBUTE_TYPE_UNDEFINED,                          \
    -1, -1)                                                     \
  X(0xc00002a4, STATUS_DS_ATTRIBUTE_OR_VALUE_EXISTS,            \
    ERROR_DS_ATTRIBUTE_OR_VALUE_EXISTS,                         \
    -1, -1)                                                     \
  X(0xc00002a5, STATUS_DS_BUSY,                                 \
    ERROR_DS_BUSY,                                              \
    -1, -1)                                                     \
  X(0xc00002a6, STATUS_DS_UNAVAILABLE,                          \
    ERROR_DS_UNAVAILABLE,                                       \
    -1, -1)                                                     \
  X(0xc00002a7, STATUS_DS_NO_RIDS_ALLOCATED,                    \
    ERROR_DS_NO_RIDS_ALLOCATED,                                 \
    -1, -1)                                                     \
  X(0xc00002a8, STATUS_DS_NO_MORE_RIDS,                         \
    ERROR_DS_NO_MORE_RIDS,                                      \
    -1, -1)                                                     \
  X(0xc00002a9, STATUS_DS_INCORRECT_ROLE_OWNER,                 \
    ERROR_DS_INCORRECT_ROLE_OWNER,                              \
    -1, -1)                                                     \
  X(0xc00002aa, STATUS_DS_RIDMGR_INIT_ERROR,                    \
    ERROR_DS_RIDMGR_INIT_ERROR,                                 \
    -1, -1)                                                     \
  X(0xc00002ab, STATUS_DS_OBJ_CLASS_VIOLATION,                  \
    ERROR_DS_OBJ_CLASS_VIOLATION,                               \
    -1, -1)                                                     \
  X(0xc00002ac, STATUS_DS_CANT_ON_NON_LEAF,                     \
    ERROR_DS_CANT_ON_NON_LEAF,                                  \
    -1, -1)                                                     \
  X(0xc00002ad, STATUS_DS_CANT_ON_RDN,                          \
    ERROR_DS_CANT_ON_RDN,                                       \
    -1, -1)                                                     \
  X(0xc00002ae, STATUS_DS_CANT_MOD_OBJ_CLASS,                   \
    ERROR_DS_CANT_MOD_OBJ_CLASS,                                \
    -1, -1)                                                     \
  X(0xc00002af, STATUS_DS_CROSS_DOM_MOVE_FAILED,                \
    ERROR_DS_CROSS_DOM_MOVE_ERROR,                              \
    -1, -1)                                                     \
  X(0xc00002b0, STATUS_DS_GC_NOT_AVAILABLE,                     \
    ERROR_DS_GC_NOT_AVAILABLE,                                  \
    -1, -1)                                                     \
  X(0xc00002b1, STATUS_DIRECTORY_SERVICE_REQUIRED,              \
    ERROR_DS_DS_REQUIRED,                                       \
    -1, -1)                                                     \
  X(0xc00002b2, STATUS_REPARSE_ATTRIBUTE_CONFLICT,              \
    ERROR_REPARSE_ATTRIBUTE_CONFLICT,                           \
    -1, -1)                                                     \
  X(0xc00002b3, STATUS_CANT_ENABLE_DENY_ONLY,                   \
    ERROR_CANT_ENABLE_DENY_ONLY,                                \
    -1, -1)                                                     \
  X(0xc00002b4, STATUS_FLOAT_MULTIPLE_FAULTS,                   \
    ERROR_FLOAT_MULTIPLE_FAULTS,                                \
    -1, -1)                                                     \
  X(0xc00002b5, STATUS_FLOAT_MULTIPLE_TRAPS,                    \
    ERROR_FLOAT_MULTIPLE_TRAPS,                                 \
    -1, -1)                                                     \
  X(0xc00002b6, STATUS_DEVICE_REMOVED,                          \
    ERROR_DEVICE_REMOVED,                                       \
    -1, -1)                                                     \
  X(0xc00002b7, STATUS_JOURNAL_DELETE_IN_PROGRESS,              \
    ERROR_JOURNAL_DELETE_IN_PROGRESS,                           \
    -1, -1)                                                     \
  X(0xc00002b8, STATUS_JOURNAL_NOT_ACTIVE,                      \
    ERROR_JOURNAL_NOT_ACTIVE,                                   \
    -1, -1)                                                     \
  X(0xc00002b9, STATUS_NOINTERFACE,                             \
    ERROR_NOINTERFACE,                                          \
    -1, -1)                                                     \
  X(0xc00002c1, STATUS_DS_ADMIN_LIMIT_EXCEEDED,                 \
    ERROR_DS_ADMIN_LIMIT_EXCEEDED,                              \
    -1, -1)                                                     \
  X(0xc00002c2, STATUS_DRIVER_FAILED_SLEEP,                     \
    ERROR_DRIVER_FAILED_SLEEP,                                  \
    -1, -1)                                                     \
  X(0xc00002c3, STATUS_MUTUAL_AUTHENTICATION_FAILED,            \
    ERROR_MUTUAL_AUTH_FAILED,                                   \
    -1, -1)                                                     \
  X(0xc00002c4, STATUS_CORRUPT_SYSTEM_FILE,                     \
    ERROR_CORRUPT_SYSTEM_FILE,                                  \
    -1, -1)                                                     \
  X(0xc00002c5, STATUS_DATATYPE_MISALIGNMENT_ERROR,             \
    ERROR_NOACCESS,                                             \
    -1, -1)                                                     \
  X(0xc00002c6, STATUS_WMI_READ_ONLY,                           \
    ERROR_WMI_READ_ONLY,                                        \
    -1, -1)                                                     \
  X(0xc00002c7, STATUS_WMI_SET_FAILURE,                         \
    ERROR_WMI_SET_FAILURE,                                      \
    -1, -1)                                                     \
  X(0xc00002c8, STATUS_COMMITMENT_MINIMUM,                      \
    ERROR_COMMITMENT_MINIMUM,                                   \
    -1, -1)                                                     \
  X(0xc00002c9, STATUS_REG_NAT_CONSUMPTION,                     \
    ERROR_REG_NAT_CONSUMPTION,                                  \
    -1, -1)                                                     \
  X(0xc00002ca, STATUS_TRANSPORT_FULL,                          \
    ERROR_TRANSPORT_FULL,                                       \
    -1, -1)                                                     \
  X(0xc00002cb, STATUS_DS_SAM_INIT_FAILURE,                     \
    ERROR_DS_SAM_INIT_FAILURE,                                  \
    -1, -1)                                                     \
  X(0xc00002cc, STATUS_ONLY_IF_CONNECTED,                       \
    ERROR_ONLY_IF_CONNECTED,                                    \
    -1, -1)                                                     \
  X(0xc00002cd, STATUS_DS_SENSITIVE_GROUP_VIOLATION,            \
    ERROR_DS_SENSITIVE_GROUP_VIOLATION,                         \
    -1, -1)                                                     \
  X(0xc00002ce, STATUS_PNP_RESTART_ENUMERATION,                 \
    ERROR_PNP_RESTART_ENUMERATION,                              \
    -1, -1)                                                     \
  X(0xc00002cf, STATUS_JOURNAL_ENTRY_DELETED,                   \
    ERROR_JOURNAL_ENTRY_DELETED,                                \
    -1, -1)                                                     \
  X(0xc00002d0, STATUS_DS_CANT_MOD_PRIMARYGROUPID,              \
    ERROR_DS_CANT_MOD_PRIMARYGROUPID,                           \
    -1, -1)                                                     \
  X(0xc00002d1, STATUS_SYSTEM_IMAGE_BAD_SIGNATURE,              \
    ERROR_SYSTEM_IMAGE_BAD_SIGNATURE,                           \
    -1, -1)                                                     \
  X(0xc00002d2, STATUS_PNP_REBOOT_REQUIRED,                     \
    ERROR_PNP_REBOOT_REQUIRED,                                  \
    -1, -1)                                                     \
  X(0xc00002d3, STATUS_POWER_STATE_INVALID,                     \
    ERROR_NOT_READY,                                            \
    -1, EACCES)                                                 \
  X(0xc00002d4, STATUS_DS_INVALID_GROUP_TYPE,                   \
    ERROR_DS_INVALID_GROUP_TYPE,                                \
    -1, -1)                                                     \
  X(0xc00002d5, STATUS_DS_NO_NEST_GLOBALGROUP_IN_MIXEDDOMAIN,   \
    ERROR_DS_NO_NEST_GLOBALGROUP_IN_MIXEDDOMAIN,                \
    -1, -1)                                                     \
  X(0xc00002d6, STATUS_DS_NO_NEST_LOCALGROUP_IN_MIXEDDOMAIN,    \
    ERROR_DS_NO_NEST_LOCALGROUP_IN_MIXEDDOMAIN,                 \
    -1, -1)                                                     \
  X(0xc00002d7, STATUS_DS_GLOBAL_CANT_HAVE_LOCAL_MEMBER,        \
    ERROR_DS_GLOBAL_CANT_HAVE_LOCAL_MEMBER,                     \
    -1, -1)                                                     \
  X(0xc00002d8, STATUS_DS_GLOBAL_CANT_HAVE_UNIVERSAL_MEMBER,    \
    ERROR_DS_GLOBAL_CANT_HAVE_UNIVERSAL_MEMBER,                 \
    -1, -1)                                                     \
  X(0xc00002d9, STATUS_DS_UNIVERSAL_CANT_HAVE_LOCAL_MEMBER,     \
    ERROR_DS_UNIVERSAL_CANT_HAVE_LOCAL_MEMBER,                  \
    -1, -1)                                                     \
  X(0xc00002da, STATUS_DS_GLOBAL_CANT_HAVE_CROSSDOMAIN_MEMBER,  \
    ERROR_DS_GLOBAL_CANT_HAVE_CROSSDOMAIN_MEMBER,               \
    -1, -1)                                                     \
  X(0xc00002db, /* ntstatus symbol not available */,            \
    ERROR_DS_LOCAL_CANT_HAVE_CROSSDOMAIN_LOCAL_MEMBER,          \
    -1, -1)                                                     \
  X(0xc00002dc, STATUS_DS_HAVE_PRIMARY_MEMBERS,                 \
    ERROR_DS_HAVE_PRIMARY_MEMBERS,                              \
    -1, -1)                                                     \
  X(0xc00002dd, STATUS_WMI_NOT_SUPPORTED,                       \
    ERROR_NOT_SUPPORTED,                                        \
    -1, -1)                                                     \
  X(0xc00002de, STATUS_INSUFFICIENT_POWER,                      \
    ERROR_INSUFFICIENT_POWER,                                   \
    -1, -1)                                                     \
  X(0xc00002df, STATUS_SAM_NEED_BOOTKEY_PASSWORD,               \
    ERROR_DS_SAM_NEED_BOOTKEY_PASSWORD,                         \
    -1, -1)                                                     \
  X(0xc00002e0, STATUS_SAM_NEED_BOOTKEY_FLOPPY,                 \
    ERROR_DS_SAM_NEED_BOOTKEY_FLOPPY,                           \
    -1, -1)                                                     \
  X(0xc00002e1, STATUS_DS_CANT_START,                           \
    ERROR_DS_CANT_START,                                        \
    -1, -1)                                                     \
  X(0xc00002e2, STATUS_DS_INIT_FAILURE,                         \
    ERROR_DS_INIT_FAILURE,                                      \
    -1, -1)                                                     \
  X(0xc00002e3, STATUS_SAM_INIT_FAILURE,                        \
    ERROR_SAM_INIT_FAILURE,                                     \
    -1, -1)                                                     \
  X(0xc00002e4, STATUS_DS_GC_REQUIRED,                          \
    ERROR_DS_GC_REQUIRED,                                       \
    -1, -1)                                                     \
  X(0xc00002e5, STATUS_DS_LOCAL_MEMBER_OF_LOCAL_ONLY,           \
    ERROR_DS_LOCAL_MEMBER_OF_LOCAL_ONLY,                        \
    -1, -1)                                                     \
  X(0xc00002e6, STATUS_DS_NO_FPO_IN_UNIVERSAL_GROUPS,           \
    ERROR_DS_NO_FPO_IN_UNIVERSAL_GROUPS,                        \
    -1, -1)                                                     \
  X(0xc00002e7, STATUS_DS_MACHINE_ACCOUNT_QUOTA_EXCEEDED,       \
    ERROR_DS_MACHINE_ACCOUNT_QUOTA_EXCEEDED,                    \
    -1, -1)                                                     \
  X(0xc00002e8, STATUS_MULTIPLE_FAULT_VIOLATION,                \
    ERROR_MULTIPLE_FAULT_VIOLATION,                             \
    -1, -1)                                                     \
  X(0xc00002e9, STATUS_CURRENT_DOMAIN_NOT_ALLOWED,              \
    ERROR_CURRENT_DOMAIN_NOT_ALLOWED,                           \
    -1, -1)                                                     \
  X(0xc00002ea, STATUS_CANNOT_MAKE,                             \
    ERROR_CANNOT_MAKE,                                          \
    -1, EACCES)                                                 \
  X(0xc00002eb, STATUS_SYSTEM_SHUTDOWN,                         \
    ERROR_SYSTEM_SHUTDOWN,                                      \
    -1, -1)                                                     \
  X(0xc00002ec, STATUS_DS_INIT_FAILURE_CONSOLE,                 \
    ERROR_DS_INIT_FAILURE_CONSOLE,                              \
    -1, -1)                                                     \
  X(0xc00002ed, STATUS_DS_SAM_INIT_FAILURE_CONSOLE,             \
    ERROR_DS_SAM_INIT_FAILURE_CONSOLE,                          \
    -1, -1)                                                     \
  X(0xc00002f0, STATUS_OBJECTID_NOT_FOUND,                      \
    ERROR_FILE_NOT_FOUND,                                       \
    -1, ENOENT)                                                 \
  X(0xc00002fe, STATUS_SHUTDOWN_IN_PROGRESS,                    \
    ERROR_SHUTDOWN_IN_PROGRESS,                                 \
    -1, -1)                                                     \
  X(0xc00002ff, STATUS_SERVER_SHUTDOWN_IN_PROGRESS,             \
    ERROR_SERVER_SHUTDOWN_IN_PROGRESS,                          \
    -1, -1)                                                     \
  X(0xc0000300, STATUS_NOT_SUPPORTED_ON_SBS,                    \
    ERROR_NOT_SUPPORTED_ON_SBS,                                 \
    -1, -1)                                                     \
  X(0xc0000301, STATUS_WMI_GUID_DISCONNECTED,                   \
    ERROR_WMI_GUID_DISCONNECTED,                                \
    -1, -1)                                                     \
  X(0xc0000302, STATUS_WMI_ALREADY_DISABLED,                    \
    ERROR_WMI_ALREADY_DISABLED,                                 \
    -1, -1)                                                     \
  X(0xc0000303, STATUS_WMI_ALREADY_ENABLED,                     \
    ERROR_WMI_ALREADY_ENABLED,                                  \
    -1, -1)                                                     \
  X(0xc0000304, STATUS_MFT_TOO_FRAGMENTED,                      \
    ERROR_DISK_TOO_FRAGMENTED,                                  \
    -1, -1)                                                     \
  X(0xc0000320, STATUS_PKINIT_FAILURE,                          \
    ERROR_PKINIT_FAILURE,                                       \
    -1, -1)                                                     \
  X(0xc0000321, STATUS_SMARTCARD_SUBSYSTEM_FAILURE,             \
    ERROR_SMARTCARD_SUBSYSTEM_FAILURE,                          \
    -1, -1)                                                     \
  X(0xc0000350, STATUS_HOST_DOWN,                               \
    ERROR_HOST_DOWN,                                            \
    -1, -1)                                                     \
  X(0xc0000352, STATUS_EFS_ALG_BLOB_TOO_BIG,                    \
    ERROR_EFS_ALG_BLOB_TOO_BIG,                                 \
    -1, -1)                                                     \
  X(0xc0000353, STATUS_PORT_NOT_SET,                            \
    ERROR_PORT_NOT_SET,                                         \
    -1, -1)                                                     \
  X(0xc0000354, STATUS_DEBUGGER_INACTIVE,                       \
    ERROR_DEBUGGER_INACTIVE,                                    \
    -1, -1)                                                     \
  X(0xc0000355, STATUS_DS_VERSION_CHECK_FAILURE,                \
    ERROR_DS_VERSION_CHECK_FAILURE,                             \
    -1, -1)                                                     \
  X(0xc0000357, STATUS_PRENT4_MACHINE_ACCOUNT,                  \
    ERROR_DS_MACHINE_ACCOUNT_CREATED_PRENT4,                    \
    -1, -1)                                                     \
  X(0xc0000358, STATUS_DS_AG_CANT_HAVE_UNIVERSAL_MEMBER,        \
    ERROR_DS_AG_CANT_HAVE_UNIVERSAL_MEMBER,                     \
    -1, -1)                                                     \
  X(0xc0000359, STATUS_INVALID_IMAGE_WIN_32,                    \
    ERROR_BAD_EXE_FORMAT,                                       \
    -1, ENOEXEC)                                                \
  X(0xc000035a, STATUS_INVALID_IMAGE_WIN_64,                    \
    ERROR_BAD_EXE_FORMAT,                                       \
    -1, ENOEXEC)                                                \
  X(0xc000035c, STATUS_NETWORK_SESSION_EXPIRED,                 \
    ERROR_NO_USER_SESSION_KEY,                                  \
    -1, -1)                                                     \
  X(0xc000035d, STATUS_APPHELP_BLOCK,                           \
    ERROR_APPHELP_BLOCK,                                        \
    -1, -1)                                                     \
  X(0xc000035f, STATUS_NOT_SAFE_MODE_DRIVER,                    \
    ERROR_NOT_SAFE_MODE_DRIVER,                                 \
    -1, -1)                                                     \
  X(0xc0000361, STATUS_ACCESS_DISABLED_BY_POLICY_DEFAULT,       \
    ERROR_ACCESS_DISABLED_BY_POLICY,                            \
    -1, -1)                                                     \
  X(0xc0000362, STATUS_ACCESS_DISABLED_BY_POLICY_PATH,          \
    ERROR_ACCESS_DISABLED_BY_POLICY,                            \
    -1, -1)                                                     \
  X(0xc0000363, STATUS_ACCESS_DISABLED_BY_POLICY_PUBLISHER,     \
    ERROR_ACCESS_DISABLED_BY_POLICY,                            \
    -1, -1)                                                     \
  X(0xc0000364, STATUS_ACCESS_DISABLED_BY_POLICY_OTHER,         \
    ERROR_ACCESS_DISABLED_BY_POLICY,                            \
    -1, -1)                                                     \
  X(0xc0000365, STATUS_FAILED_DRIVER_ENTRY,                     \
    ERROR_FAILED_DRIVER_ENTRY,                                  \
    -1, -1)                                                     \
  X(0xc0000366, STATUS_DEVICE_ENUMERATION_ERROR,                \
    ERROR_DEVICE_ENUMERATION_ERROR,                             \
    -1, -1)                                                     \
  X(0xc0000368, STATUS_MOUNT_POINT_NOT_RESOLVED,                \
    ERROR_MOUNT_POINT_NOT_RESOLVED,                             \
    -1, -1)                                                     \
  X(0xc0000369, STATUS_INVALID_DEVICE_OBJECT_PARAMETER,         \
    ERROR_INVALID_DEVICE_OBJECT_PARAMETER,                      \
    -1, -1)                                                     \
  X(0xc000036a, STATUS_MCA_OCCURED,                             \
    ERROR_MCA_OCCURED,                                          \
    -1, -1)                                                     \
  X(0xc000036b, STATUS_DRIVER_BLOCKED_CRITICAL,                 \
    ERROR_DRIVER_BLOCKED,                                       \
    -1, -1)                                                     \
  X(0xc000036c, STATUS_DRIVER_BLOCKED,                          \
    ERROR_DRIVER_BLOCKED,                                       \
    -1, -1)                                                     \
  X(0xc000036d, STATUS_DRIVER_DATABASE_ERROR,                   \
    ERROR_DRIVER_DATABASE_ERROR,                                \
    -1, -1)                                                     \
  X(0xc000036e, STATUS_SYSTEM_HIVE_TOO_LARGE,                   \
    ERROR_SYSTEM_HIVE_TOO_LARGE,                                \
    -1, -1)                                                     \
  X(0xc000036f, STATUS_INVALID_IMPORT_OF_NON_DLL,               \
    ERROR_INVALID_IMPORT_OF_NON_DLL,                            \
    -1, -1)                                                     \
  X(0xc0000371, STATUS_NO_SECRETS,                              \
    ERROR_NO_SECRETS,                                           \
    -1, -1)                                                     \
  X(0xc0000372, STATUS_ACCESS_DISABLED_NO_SAFER_UI_BY_POLICY,   \
    ERROR_ACCESS_DISABLED_NO_SAFER_UI_BY_POLICY,                \
    -1, -1)                                                     \
  X(0xc0000373, STATUS_FAILED_STACK_SWITCH,                     \
    ERROR_NOT_ENOUGH_MEMORY,                                    \
    -1, ENOMEM)                                                 \
  X(0xc0000374, STATUS_HEAP_CORRUPTION,                         \
    ERROR_INTERNAL_ERROR,                                       \
    -1, -1)                                                     \
  X(0xc0000388, STATUS_DOWNGRADE_DETECTED,                      \
    ERROR_DOWNGRADE_DETECTED,                                   \
    -1, -1)                                                     \
  X(0xc000038e, STATUS_DRIVER_FAILED_PRIOR_UNLOAD,              \
    ERROR_DRIVER_FAILED_PRIOR_UNLOAD,                           \
    -1, -1)                                                     \
  X(0xc0000401, STATUS_PER_USER_TRUST_QUOTA_EXCEEDED,           \
    ERROR_PER_USER_TRUST_QUOTA_EXCEEDED,                        \
    -1, -1)                                                     \
  X(0xc0000402, STATUS_ALL_USER_TRUST_QUOTA_EXCEEDED,           \
    ERROR_ALL_USER_TRUST_QUOTA_EXCEEDED,                        \
    -1, -1)                                                     \
  X(0xc0000403, STATUS_USER_DELETE_TRUST_QUOTA_EXCEEDED,        \
    ERROR_USER_DELETE_TRUST_QUOTA_EXCEEDED,                     \
    -1, -1)                                                     \
  X(0xc0000404, STATUS_DS_NAME_NOT_UNIQUE,                      \
    ERROR_DS_NAME_NOT_UNIQUE,                                   \
    -1, -1)                                                     \
  X(0xc0000405, STATUS_DS_DUPLICATE_ID_FOUND,                   \
    ERROR_DS_DUPLICATE_ID_FOUND,                                \
    -1, -1)                                                     \
  X(0xc0000406, STATUS_DS_GROUP_CONVERSION_ERROR,               \
    ERROR_DS_GROUP_CONVERSION_ERROR,                            \
    -1, -1)                                                     \
  X(0xc0000407, STATUS_VOLSNAP_PREPARE_HIBERNATE,               \
    ERROR_VOLSNAP_PREPARE_HIBERNATE,                            \
    -1, -1)                                                     \
  X(0xc0000408, STATUS_USER2USER_REQUIRED,                      \
    ERROR_LOGON_FAILURE,                                        \
    -1, -1)                                                     \
  X(0xc0000409, STATUS_STACK_BUFFER_OVERRUN,                    \
    ERROR_STACK_BUFFER_OVERRUN,                                 \
    -1, -1)                                                     \
  X(0xc0000410, STATUS_PARAMETER_QUOTA_EXCEEDED,                \
    ERROR_PARAMETER_QUOTA_EXCEEDED,                             \
    -1, -1)                                                     \
  X(0xc0000411, STATUS_HIBERNATION_FAILURE,                     \
    ERROR_HIBERNATION_FAILURE,                                  \
    -1, -1)                                                     \
  X(0xc0000412, STATUS_DELAY_LOAD_FAILED,                       \
    ERROR_DELAY_LOAD_FAILED,                                    \
    -1, -1)                                                     \
  X(0xc0000413, STATUS_AUTHENTICATION_FIREWALL_FAILED,          \
    ERROR_AUTHENTICATION_FIREWALL_FAILED,                       \
    -1, -1)                                                     \
  X(0xc0000414, STATUS_VDM_DISALLOWED,                          \
    ERROR_VDM_DISALLOWED,                                       \
    -1, -1)                                                     \
  X(0xc0000416, /* ntstatus symbol not available */,            \
    ERROR_NOT_ENOUGH_MEMORY,                                    \
    -1, ENOMEM)                                                 \
  X(0xc0000417, STATUS_INVALID_CRUNTIME_PARAMETER,              \
    ERROR_INVALID_CRUNTIME_PARAMETER,                           \
    -1, -1)                                                     \
  X(0xc0000418, STATUS_NTLM_BLOCKED,                            \
    ERROR_NTLM_BLOCKED,                                         \
    -1, -1)                                                     \
  X(0xc0000419, STATUS_DS_SRC_SID_EXISTS_IN_FOREST,             \
    ERROR_DS_SRC_SID_EXISTS_IN_FOREST,                          \
    -1, -1)                                                     \
  X(0xc000041a, STATUS_DS_DOMAIN_NAME_EXISTS_IN_FOREST,         \
    ERROR_DS_DOMAIN_NAME_EXISTS_IN_FOREST,                      \
    -1, -1)                                                     \
  X(0xc000041b, STATUS_DS_FLAT_NAME_EXISTS_IN_FOREST,           \
    ERROR_DS_FLAT_NAME_EXISTS_IN_FOREST,                        \
    -1, -1)                                                     \
  X(0xc000041c, STATUS_INVALID_USER_PRINCIPAL_NAME,             \
    ERROR_INVALID_USER_PRINCIPAL_NAME,                          \
    -1, -1)                                                     \
  X(0xc000041d, STATUS_FATAL_USER_CALLBACK_EXCEPTION,           \
    ERROR_FATAL_APP_EXIT,                                       \
    -1, -1)                                                     \
  X(0xc0000420, STATUS_ASSERTION_FAILURE,                       \
    ERROR_ASSERTION_FAILURE,                                    \
    -1, -1)                                                     \
  X(0xc0000421, STATUS_VERIFIER_STOP,                           \
    ERROR_VERIFIER_STOP,                                        \
    -1, -1)                                                     \
  X(0xc0000423, STATUS_CALLBACK_POP_STACK,                      \
    ERROR_CALLBACK_POP_STACK,                                   \
    -1, -1)                                                     \
  X(0xc0000424, STATUS_INCOMPATIBLE_DRIVER_BLOCKED,             \
    ERROR_DRIVER_BLOCKED,                                       \
    -1, -1)                                                     \
  X(0xc0000425, STATUS_HIVE_UNLOADED,                           \
    ERROR_KEY_DELETED,                                          \
    -1, -1)                                                     \
  X(0xc0000426, STATUS_COMPRESSION_DISABLED,                    \
    ERROR_COMPRESSION_DISABLED,                                 \
    -1, -1)                                                     \
  X(0xc0000427, STATUS_FILE_SYSTEM_LIMITATION,                  \
    ERROR_FILE_SYSTEM_LIMITATION,                               \
    -1, -1)                                                     \
  X(0xc0000428, STATUS_INVALID_IMAGE_HASH,                      \
    ERROR_INVALID_IMAGE_HASH,                                   \
    -1, -1)                                                     \
  X(0xc0000429, STATUS_NOT_CAPABLE,                             \
    ERROR_NOT_CAPABLE,                                          \
    -1, -1)                                                     \
  X(0xc000042a, STATUS_REQUEST_OUT_OF_SEQUENCE,                 \
    ERROR_REQUEST_OUT_OF_SEQUENCE,                              \
    -1, -1)                                                     \
  X(0xc000042b, STATUS_IMPLEMENTATION_LIMIT,                    \
    ERROR_IMPLEMENTATION_LIMIT,                                 \
    -1, -1)                                                     \
  X(0xc000042c, STATUS_ELEVATION_REQUIRED,                      \
    ERROR_ELEVATION_REQUIRED,                                   \
    -1, -1)                                                     \
  X(0xc0000432, STATUS_BEYOND_VDL,                              \
    ERROR_BEYOND_VDL,                                           \
    -1, -1)                                                     \
  X(0xc0000433, STATUS_ENCOUNTERED_WRITE_IN_PROGRESS,           \
    ERROR_BUSY,                                                 \
    -1, -1)                                                     \
  X(0xc0000434, STATUS_PTE_CHANGED,                             \
    ERROR_BUSY,                                                 \
    -1, -1)                                                     \
  X(0xc0000435, STATUS_PURGE_FAILED,                            \
    ERROR_USER_MAPPED_FILE,                                     \
    -1, -1)                                                     \
  X(0xc0000441, STATUS_CS_ENCRYPTION_INVALID_SERVER_RESPONSE,   \
    ERROR_CS_ENCRYPTION_INVALID_SERVER_RESPONSE,                \
    -1, -1)                                                     \
  X(0xc0000442, STATUS_CS_ENCRYPTION_UNSUPPORTED_SERVER,        \
    ERROR_CS_ENCRYPTION_UNSUPPORTED_SERVER,                     \
    -1, -1)                                                     \
  X(0xc0000443, STATUS_CS_ENCRYPTION_EXISTING_ENCRYPTED_FILE,   \
    ERROR_CS_ENCRYPTION_EXISTING_ENCRYPTED_FILE,                \
    -1, -1)                                                     \
  X(0xc0000444, STATUS_CS_ENCRYPTION_NEW_ENCRYPTED_FILE,        \
    ERROR_CS_ENCRYPTION_NEW_ENCRYPTED_FILE,                     \
    -1, -1)                                                     \
  X(0xc0000445, STATUS_CS_ENCRYPTION_FILE_NOT_CSE,              \
    ERROR_CS_ENCRYPTION_FILE_NOT_CSE,                           \
    -1, -1)                                                     \
  X(0xc0000446, STATUS_INVALID_LABEL,                           \
    ERROR_INVALID_LABEL,                                        \
    -1, -1)                                                     \
  X(0xc0000450, STATUS_DRIVER_PROCESS_TERMINATED,               \
    ERROR_DRIVER_PROCESS_TERMINATED,                            \
    -1, -1)                                                     \
  X(0xc0000451, STATUS_AMBIGUOUS_SYSTEM_DEVICE,                 \
    ERROR_AMBIGUOUS_SYSTEM_DEVICE,                              \
    -1, -1)                                                     \
  X(0xc0000452, STATUS_SYSTEM_DEVICE_NOT_FOUND,                 \
    ERROR_SYSTEM_DEVICE_NOT_FOUND,                              \
    -1, -1)                                                     \
  X(0xc0000453, STATUS_RESTART_BOOT_APPLICATION,                \
    ERROR_RESTART_APPLICATION,                                  \
    -1, -1)                                                     \
  X(0xc0000454, STATUS_INSUFFICIENT_NVRAM_RESOURCES,            \
    ERROR_NO_NVRAM_RESOURCES,                                   \
    -1, -1)                                                     \
  X(0xc0000455, /* ntstatus symbol not available */,            \
    ERROR_INVALID_HANDLE,                                       \
    -1, EBADF)                                                  \
  X(0xc0000456, /* ntstatus symbol not available */,            \
    ERROR_INVALID_PARAMETER,                                    \
    -1, EINVAL)                                                 \
  X(0xc0000457, /* ntstatus symbol not available */,            \
    ERROR_INVALID_PARAMETER,                                    \
    -1, EINVAL)                                                 \
  X(0xc0000458, /* ntstatus symbol not available */,            \
    ERROR_INVALID_PARAMETER,                                    \
    -1, EINVAL)                                                 \
  X(0xc0000466, /* ntstatus symbol not available */,            \
    ERROR_NETNAME_DELETED,                                      \
    -1, -1)                                                     \
  X(0xc0000467, /* ntstatus symbol not available */,            \
    ERROR_SHARING_VIOLATION,                                    \
    -1, EACCES)                                                 \
  X(0xc000047d, /* ntstatus symbol not available */,            \
    ERROR_TIMEOUT,                                              \
    -1, -1)                                                     \
  X(0xc0000480, /* ntstatus symbol not available */,            \
    ERROR_NETNAME_DELETED,                                      \
    -1, -1)                                                     \
  X(0xc0000481, /* ntstatus symbol not available */,            \
    ERROR_MOD_NOT_FOUND,                                        \
    -1, -1)                                                     \
  X(0xc0000482, /* ntstatus symbol not available */,            \
    ERROR_MOD_NOT_FOUND,                                        \
    -1, -1)                                                     \
  X(0xc0000487, /* ntstatus symbol not available */,            \
    ERROR_GEN_FAILURE,                                          \
    -1, EACCES)                                                 \
  X(0xc0000490, /* ntstatus symbol not available */,            \
    ERROR_NO_MATCH,                                             \
    -1, -1)                                                     \
  X(0xc0000491, /* ntstatus symbol not available */,            \
    ERROR_FILE_NOT_FOUND,                                       \
    -1, ENOENT)                                                 \
  X(0xc0000492, /* ntstatus symbol not available */,            \
    ERROR_NOT_FOUND,                                            \
    -1, -1)                                                     \
  X(0xc0000493, /* ntstatus symbol not available */,            \
    ERROR_SET_NOT_FOUND,                                        \
    -1, -1)                                                     \
  X(0xc0000494, /* ntstatus symbol not available */,            \
    ERROR_NOT_CAPABLE,                                          \
    -1, -1)                                                     \
  X(0xc0000495, /* ntstatus symbol not available */,            \
    ERROR_NOT_READY,                                            \
    -1, EACCES)                                                 \
  X(0xc000049b, /* ntstatus symbol not available */,            \
    ERROR_DISK_TOO_FRAGMENTED,                                  \
    -1, -1)                                                     \
  X(0xc0000500, STATUS_INVALID_TASK_NAME,                       \
    ERROR_INVALID_TASK_NAME,                                    \
    -1, -1)                                                     \
  X(0xc0000501, STATUS_INVALID_TASK_INDEX,                      \
    ERROR_INVALID_TASK_INDEX,                                   \
    -1, -1)                                                     \
  X(0xc0000502, STATUS_THREAD_ALREADY_IN_TASK,                  \
    ERROR_THREAD_ALREADY_IN_TASK,                               \
    -1, -1)                                                     \
  X(0xc0000503, STATUS_CALLBACK_BYPASS,                         \
    ERROR_NOT_READY,                                            \
    -1, EACCES)                                                 \
  X(0xc0000507, /* ntstatus symbol not available */,            \
    ERROR_BUSY,                                                 \
    -1, -1)                                                     \
  X(0xc000050f, /* ntstatus symbol not available */,            \
    ERROR_NOT_EMPTY,                                            \
    -1, -1)                                                     \
  X(0xc0000510, /* ntstatus symbol not available */,            \
    ERROR_ALREADY_INITIALIZED,                                  \
    -1, -1)                                                     \
  X(0xc0000602, STATUS_FAIL_FAST_EXCEPTION,                     \
    ERROR_FAIL_FAST_EXCEPTION,                                  \
    -1, -1)                                                     \
  X(0xc0000700, STATUS_PORT_CLOSED,                             \
    ERROR_INTERNAL_ERROR,                                       \
    -1, -1)                                                     \
  X(0xc0000701, STATUS_MESSAGE_LOST,                            \
    ERROR_INTERNAL_ERROR,                                       \
    -1, -1)                                                     \
  X(0xc0000702, STATUS_INVALID_MESSAGE,                         \
    ERROR_INVALID_PARAMETER,                                    \
    -1, EINVAL)                                                 \
  X(0xc0000703, STATUS_REQUEST_CANCELED,                        \
    ERROR_INTERNAL_ERROR,                                       \
    -1, -1)                                                     \
  X(0xc0000704, STATUS_RECURSIVE_DISPATCH,                      \
    ERROR_NOT_SUPPORTED,                                        \
    -1, -1)                                                     \
  X(0xc0000705, STATUS_LPC_RECEIVE_BUFFER_EXPECTED,             \
    ERROR_INVALID_PARAMETER,                                    \
    -1, EINVAL)                                                 \
  X(0xc0000706, STATUS_LPC_INVALID_CONNECTION_USAGE,            \
    ERROR_INVALID_PARAMETER,                                    \
    -1, EINVAL)                                                 \
  X(0xc0000707, STATUS_LPC_REQUESTS_NOT_ALLOWED,                \
    ERROR_NOT_SUPPORTED,                                        \
    -1, -1)                                                     \
  X(0xc0000708, STATUS_RESOURCE_IN_USE,                         \
    ERROR_INTERNAL_ERROR,                                       \
    -1, -1)                                                     \
  X(0xc0000709, STATUS_HARDWARE_MEMORY_ERROR,                   \
    ERROR_MEMORY_HARDWARE,                                      \
    -1, -1)                                                     \
  X(0xc000070a, STATUS_THREADPOOL_HANDLE_EXCEPTION,             \
    ERROR_INVALID_HANDLE,                                       \
    -1, EBADF)                                                  \
  X(0xc000070b, /* ntstatus symbol not available */,            \
    ERROR_INVALID_HANDLE,                                       \
    -1, EBADF)                                                  \
  X(0xc000070c, /* ntstatus symbol not available */,            \
    ERROR_INVALID_HANDLE,                                       \
    -1, EBADF)                                                  \
  X(0xc000070d, /* ntstatus symbol not available */,            \
    ERROR_INVALID_HANDLE,                                       \
    -1, EBADF)                                                  \
  X(0xc000070e, /* ntstatus symbol not available */,            \
    ERROR_INVALID_HANDLE,                                       \
    -1, EBADF)                                                  \
  X(0xc000070f, STATUS_THREADPOOL_RELEASED_DURING_OPERATION,    \
    ERROR_INVALID_HANDLE,                                       \
    -1, EBADF)                                                  \
  X(0xc0000710, STATUS_CALLBACK_RETURNED_WHILE_IMPERSONATING,   \
    ERROR_INVALID_FUNCTION,                                     \
    -1, EINVAL)                                                 \
  X(0xc0000711, STATUS_APC_RETURNED_WHILE_IMPERSONATING,        \
    ERROR_INVALID_FUNCTION,                                     \
    -1, EINVAL)                                                 \
  X(0xc0000712, STATUS_PROCESS_IS_PROTECTED,                    \
    ERROR_PROCESS_IS_PROTECTED,                                 \
    -1, -1)                                                     \
  X(0xc0000713, STATUS_MCA_EXCEPTION,                           \
    ERROR_MCA_EXCEPTION,                                        \
    -1, -1)                                                     \
  X(0xc0000714, STATUS_CERTIFICATE_MAPPING_NOT_UNIQUE,          \
    ERROR_LOGON_FAILURE,                                        \
    -1, -1)                                                     \
  X(0xc0000715, STATUS_SYMLINK_CLASS_DISABLED,                  \
    ERROR_SYMLINK_CLASS_DISABLED,                               \
    -1, -1)                                                     \
  X(0xc0000716, STATUS_INVALID_IDN_NORMALIZATION,               \
    ERROR_INVALID_NAME,                                         \
    -1, -1)                                                     \
  X(0xc0000717, STATUS_NO_UNICODE_TRANSLATION,                  \
    ERROR_NO_UNICODE_TRANSLATION,                               \
    -1, -1)                                                     \
  X(0xc0000718, STATUS_ALREADY_REGISTERED,                      \
    ERROR_INTERNAL_ERROR,                                       \
    -1, -1)                                                     \
  X(0xc0000719, STATUS_CONTEXT_MISMATCH,                        \
    ERROR_INTERNAL_ERROR,                                       \
    -1, -1)                                                     \
  X(0xc000071a, STATUS_PORT_ALREADY_HAS_COMPLETION_LIST,        \
    ERROR_INTERNAL_ERROR,                                       \
    -1, -1)                                                     \
  X(0xc000071b, STATUS_CALLBACK_RETURNED_THREAD_PRIORITY,       \
    ERROR_INVALID_FUNCTION,                                     \
    -1, EINVAL)                                                 \
  X(0xc000071c, STATUS_INVALID_THREAD,                          \
    ERROR_INVALID_PARAMETER,                                    \
    -1, EINVAL)                                                 \
  X(0xc000071d, STATUS_CALLBACK_RETURNED_TRANSACTION,           \
    ERROR_INVALID_FUNCTION,                                     \
    -1, EINVAL)                                                 \
  X(0xc000071e, STATUS_CALLBACK_RETURNED_LDR_LOCK,              \
    ERROR_INVALID_FUNCTION,                                     \
    -1, EINVAL)                                                 \
  X(0xc000071f, STATUS_CALLBACK_RETURNED_LANG,                  \
    ERROR_INVALID_FUNCTION,                                     \
    -1, EINVAL)                                                 \
  X(0xc0000720, STATUS_CALLBACK_RETURNED_PRI_BACK,              \
    ERROR_INVALID_FUNCTION,                                     \
    -1, EINVAL)                                                 \
  X(0xc0000721, STATUS_CALLBACK_RETURNED_THREAD_AFFINITY,       \
    ERROR_INVALID_FUNCTION,                                     \
    -1, EINVAL)                                                 \
  X(0xc0000800, STATUS_DISK_REPAIR_DISABLED,                    \
    ERROR_DISK_REPAIR_DISABLED,                                 \
    -1, -1)                                                     \
  X(0xc0000801, STATUS_DS_DOMAIN_RENAME_IN_PROGRESS,            \
    ERROR_DS_DOMAIN_RENAME_IN_PROGRESS,                         \
    -1, -1)                                                     \
  X(0xc0000802, STATUS_DISK_QUOTA_EXCEEDED,                     \
    ERROR_DISK_QUOTA_EXCEEDED,                                  \
    -1, -1)                                                     \
  X(0xc0000804, STATUS_CONTENT_BLOCKED,                         \
    ERROR_CONTENT_BLOCKED,                                      \
    -1, -1)                                                     \
  X(0xc0000805, STATUS_BAD_CLUSTERS,                            \
    ERROR_BAD_CLUSTERS,                                         \
    -1, -1)                                                     \
  X(0xc0000806, STATUS_VOLUME_DIRTY,                            \
    ERROR_VOLUME_DIRTY,                                         \
    -1, -1)                                                     \
  X(0xc000080f, /* ntstatus symbol not available */,            \
    ERROR_RETRY,                                                \
    -1, -1)                                                     \
  X(0xc0000811, /* ntstatus symbol not available */,            \
    ERROR_INTERNAL_ERROR,                                       \
    -1, -1)                                                     \
  X(0xc0000901, STATUS_FILE_CHECKED_OUT,                        \
    ERROR_FILE_CHECKED_OUT,                                     \
    -1, -1)                                                     \
  X(0xc0000902, STATUS_CHECKOUT_REQUIRED,                       \
    ERROR_CHECKOUT_REQUIRED,                                    \
    -1, -1)                                                     \
  X(0xc0000903, STATUS_BAD_FILE_TYPE,                           \
    ERROR_BAD_FILE_TYPE,                                        \
    -1, -1)                                                     \
  X(0xc0000904, STATUS_FILE_TOO_LARGE,                          \
    ERROR_FILE_TOO_LARGE,                                       \
    -1, -1)                                                     \
  X(0xc0000905, STATUS_FORMS_AUTH_REQUIRED,                     \
    ERROR_FORMS_AUTH_REQUIRED,                                  \
    -1, -1)                                                     \
  X(0xc0000906, STATUS_VIRUS_INFECTED,                          \
    ERROR_VIRUS_INFECTED,                                       \
    -1, -1)                                                     \
  X(0xc0000907, STATUS_VIRUS_DELETED,                           \
    ERROR_VIRUS_DELETED,                                        \
    -1, -1)                                                     \
  X(0xc0000908, STATUS_BAD_MCFG_TABLE,                          \
    ERROR_BAD_MCFG_TABLE,                                       \
    -1, -1)                                                     \
  X(0xc0000909, STATUS_CANNOT_BREAK_OPLOCK,                     \
    ERROR_CANNOT_BREAK_OPLOCK,                                  \
    -1, -1)                                                     \
  X(0xc0009898, STATUS_WOW_ASSERTION,                           \
    ERROR_WOW_ASSERTION,                                        \
    -1, -1)                                                     \
  X(0xc000a002, STATUS_AUTH_TAG_MISMATCH,                       \
    ERROR_CRC,                                                  \
    -1, EACCES)                                                 \
  X(0xc000a003, /* ntstatus symbol not available */,            \
    ERROR_INVALID_STATE,                                        \
    -1, -1)                                                     \
  X(0xc000a007, /* ntstatus symbol not available */,            \
    ERROR_NOT_SUPPORTED,                                        \
    -1, -1)                                                     \
  X(0xc000a010, STATUS_IPSEC_QUEUE_OVERFLOW,                    \
    ERROR_MORE_DATA,                                            \
    -1, -1)                                                     \
  X(0xc000a011, STATUS_ND_QUEUE_OVERFLOW,                       \
    ERROR_MORE_DATA,                                            \
    -1, -1)                                                     \
  X(0xc000a012, STATUS_HOPLIMIT_EXCEEDED,                       \
    ERROR_HOST_UNREACHABLE,                                     \
    WSAECONNRESET, ECONNRESET)                                  \
  X(0xc000a013, STATUS_PROTOCOL_NOT_SUPPORTED,                  \
    ERROR_NOT_SUPPORTED,                                        \
    -1, -1)                                                     \
  X(0xc000a014, STATUS_FASTPATH_REJECTED,                       \
    ERROR_PROTOCOL_UNREACHABLE,                                 \
    -1, -1)                                                     \
  X(0xc000a080, /* ntstatus symbol not available */,            \
    ERROR_LOST_WRITEBEHIND_DATA_NETWORK_DISCONNECTED,           \
    -1, -1)                                                     \
  X(0xc000a081, /* ntstatus symbol not available */,            \
    ERROR_LOST_WRITEBEHIND_DATA_NETWORK_SERVER_ERROR,           \
    -1, -1)                                                     \
  X(0xc000a082, STATUS_LOST_WRITEBEHIND_DATA_LOCAL_DISK_ERROR,  \
    ERROR_LOST_WRITEBEHIND_DATA_LOCAL_DISK_ERROR,               \
    -1, -1)                                                     \
  X(0xc000a083, STATUS_XML_PARSE_ERROR,                         \
    ERROR_XML_PARSE_ERROR,                                      \
    -1, -1)                                                     \
  X(0xc000a084, STATUS_XMLDSIG_ERROR,                           \
    ERROR_XMLDSIG_ERROR,                                        \
    -1, -1)                                                     \
  X(0xc000a085, STATUS_WRONG_COMPARTMENT,                       \
    ERROR_WRONG_COMPARTMENT,                                    \
    -1, -1)                                                     \
  X(0xc000a086, STATUS_AUTHIP_FAILURE,                          \
    ERROR_AUTHIP_FAILURE,                                       \
    -1, -1)                                                     \
  X(0xc000a087, STATUS_DS_OID_MAPPED_GROUP_CANT_HAVE_MEMBERS,   \
    ERROR_DS_OID_MAPPED_GROUP_CANT_HAVE_MEMBERS,                \
    -1, -1)                                                     \
  X(0xc000a088, STATUS_DS_OID_NOT_FOUND,                        \
    ERROR_DS_OID_NOT_FOUND,                                     \
    -1, -1)                                                     \
  X(0xc000a100, STATUS_HASH_NOT_SUPPORTED,                      \
    ERROR_HASH_NOT_SUPPORTED,                                   \
    -1, -1)                                                     \
  X(0xc000a101, STATUS_HASH_NOT_PRESENT,                        \
    ERROR_HASH_NOT_PRESENT,                                     \
    -1, -1)                                                     \
  X(0xc000a203, /* ntstatus symbol not available */,            \
    ERROR_ACCESS_DENIED,                                        \
    -1, EACCES)

/* clang-format on */

#endif /* ERROR_MAP_H_ */
