#include <malloc.h>
#include <stdlib.h>

#include "afd.h"
#include "error.h"
#include "nt.h"
#include "util.h"
#include "win.h"
#include "ws.h"

#define IOCTL_AFD_POLL 0x00012024

static UNICODE_STRING afd__helper_name =
    RTL_CONSTANT_STRING(L"\\Device\\Afd\\Wepoll");

static OBJECT_ATTRIBUTES afd__helper_attributes =
    RTL_CONSTANT_OBJECT_ATTRIBUTES(&afd__helper_name, 0);

int afd_create_helper_handle(HANDLE iocp, HANDLE* afd_helper_handle_out) {
  HANDLE afd_helper_handle;
  IO_STATUS_BLOCK iosb;
  NTSTATUS status;

  /* By opening \Device\Afd without specifying any extended attributes, we'll
   * get a handle that lets us talk to the AFD driver, but that doesn't have an
   * associated endpoint (so it's not a socket). */
  status = NtCreateFile(&afd_helper_handle,
                        SYNCHRONIZE,
                        &afd__helper_attributes,
                        &iosb,
                        NULL,
                        0,
                        FILE_SHARE_READ | FILE_SHARE_WRITE,
                        FILE_OPEN,
                        0,
                        NULL,
                        0);
  if (status != STATUS_SUCCESS)
    return_set_error(-1, RtlNtStatusToDosError(status));

  if (CreateIoCompletionPort(afd_helper_handle, iocp, 0, 0) == NULL)
    goto error;

  if (!SetFileCompletionNotificationModes(afd_helper_handle,
                                          FILE_SKIP_SET_EVENT_ON_HANDLE))
    goto error;

  *afd_helper_handle_out = afd_helper_handle;
  return 0;

error:
  CloseHandle(afd_helper_handle);
  return_map_error(-1);
}

int afd_poll(HANDLE afd_helper_handle,
             AFD_POLL_INFO* poll_info,
             OVERLAPPED* overlapped) {
  IO_STATUS_BLOCK* iosb;
  HANDLE event;
  void* apc_context;
  NTSTATUS status;

  /* Blocking operation is not supported. */
  assert(overlapped != NULL);

  iosb = (IO_STATUS_BLOCK*) &overlapped->Internal;
  event = overlapped->hEvent;

  /* Do what other windows APIs would do: if hEvent has it's lowest bit set,
   * don't post a completion to the completion port. */
  if ((uintptr_t) event & 1) {
    event = (HANDLE)((uintptr_t) event & ~(uintptr_t) 1);
    apc_context = NULL;
  } else {
    apc_context = overlapped;
  }

  iosb->Status = STATUS_PENDING;
  status = NtDeviceIoControlFile(afd_helper_handle,
                                 event,
                                 NULL,
                                 apc_context,
                                 iosb,
                                 IOCTL_AFD_POLL,
                                 poll_info,
                                 sizeof *poll_info,
                                 poll_info,
                                 sizeof *poll_info);

  if (status == STATUS_SUCCESS)
    return 0;
  else if (status == STATUS_PENDING)
    return_set_error(-1, ERROR_IO_PENDING);
  else
    return_set_error(-1, RtlNtStatusToDosError(status));
}
