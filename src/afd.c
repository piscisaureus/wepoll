#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#include "afd.h"
#include "error.h"
#include "nt.h"
#include "win.h"

#define IOCTL_AFD_POLL 0x00012024

static UNICODE_STRING afd__helper_name =
    RTL_CONSTANT_STRING(L"\\Device\\Afd\\Wepoll");

static OBJECT_ATTRIBUTES afd__helper_attributes =
    RTL_CONSTANT_OBJECT_ATTRIBUTES(&afd__helper_name, 0);

int afd_create_helper_handle(HANDLE iocp_handle,
                             HANDLE* afd_helper_handle_out) {
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

  if (CreateIoCompletionPort(afd_helper_handle, iocp_handle, 0, 0) == NULL)
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
             IO_STATUS_BLOCK* io_status_block) {
  NTSTATUS status;

  /* Blocking operation is not supported. */
  assert(io_status_block != NULL);

  io_status_block->Status = STATUS_PENDING;
  status = NtDeviceIoControlFile(afd_helper_handle,
                                 NULL,
                                 NULL,
                                 io_status_block,
                                 io_status_block,
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

int afd_cancel_poll(HANDLE afd_helper_handle,
                    IO_STATUS_BLOCK* io_status_block) {
  NTSTATUS cancel_status;
  IO_STATUS_BLOCK cancel_iosb;

  /* If the poll operation has already completed or has been cancelled earlier,
   * there's nothing left for us to do. */
  if (io_status_block->Status != STATUS_PENDING)
    return 0;

  cancel_status =
      NtCancelIoFileEx(afd_helper_handle, io_status_block, &cancel_iosb);

  /* NtCancelIoFileEx() may return STATUS_NOT_FOUND if the operation completed
   * just before calling NtCancelIoFileEx(). This is not an error. */
  if (cancel_status == STATUS_SUCCESS || cancel_status == STATUS_NOT_FOUND)
    return 0;
  else
    return_set_error(-1, RtlNtStatusToDosError(cancel_status));
}
