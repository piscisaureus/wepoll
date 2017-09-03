#include "afd.h"
#include "error.h"
#include "win.h"

int afd_poll(SOCKET socket, AFD_POLL_INFO* info, OVERLAPPED* overlapped) {
  IO_STATUS_BLOCK iosb;
  IO_STATUS_BLOCK* iosb_ptr;
  HANDLE event = NULL;
  void* apc_context;
  NTSTATUS status;
  DWORD error;

  if (overlapped != NULL) {
    /* Overlapped operation. */
    iosb_ptr = (IO_STATUS_BLOCK*) &overlapped->Internal;
    event = overlapped->hEvent;

    /* Do not report iocp completion if hEvent is tagged. */
    if ((uintptr_t) event & 1) {
      event = (HANDLE)((uintptr_t) event & ~(uintptr_t) 1);
      apc_context = NULL;
    } else {
      apc_context = overlapped;
    }

  } else {
    /* Blocking operation. */
    iosb_ptr = &iosb;
    event = CreateEventW(NULL, FALSE, FALSE, NULL);
    if (event == NULL) {
      return SOCKET_ERROR;
    }
    apc_context = NULL;
  }

  iosb_ptr->Status = STATUS_PENDING;
  status = NtDeviceIoControlFile((HANDLE) socket,
                                 event,
                                 NULL,
                                 apc_context,
                                 iosb_ptr,
                                 IOCTL_AFD_POLL,
                                 info,
                                 sizeof *info,
                                 info,
                                 sizeof *info);

  if (overlapped == NULL) {
    /* If this is a blocking operation, wait for the event to become
     * signaled, and then grab the real status from the io status block.
     */
    if (status == STATUS_PENDING) {
      DWORD r = WaitForSingleObject(event, INFINITE);

      if (r == WAIT_FAILED) {
        DWORD saved_error = GetLastError();
        CloseHandle(event);
        WSASetLastError(saved_error);
        return SOCKET_ERROR;
      }

      status = iosb_ptr->Status;
    }

    CloseHandle(event);
  }

  switch (status) {
    case STATUS_SUCCESS:
      error = ERROR_SUCCESS;
      break;

    case STATUS_PENDING:
      error = WSA_IO_PENDING;
      break;

    default:
      error = we_map_ntstatus_to_winsock_error(status);
      break;
  }

  WSASetLastError(error);

  if (error == ERROR_SUCCESS) {
    return 0;
  } else {
    return SOCKET_ERROR;
  }
}
