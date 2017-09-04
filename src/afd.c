#include "afd.h"
#include "error.h"
#include "nt.h"
#include "win.h"

#define FILE_DEVICE_NETWORK 0x00000012
#define METHOD_BUFFERED 0
#define AFD_POLL 9

#define _AFD_CONTROL_CODE(operation, method) \
  ((FILE_DEVICE_NETWORK) << 12 | (operation << 2) | method)

#define IOCTL_AFD_POLL _AFD_CONTROL_CODE(AFD_POLL, METHOD_BUFFERED)

int afd_poll(SOCKET socket, AFD_POLL_INFO* info, OVERLAPPED* overlapped) {
  IO_STATUS_BLOCK iosb;
  IO_STATUS_BLOCK* iosb_ptr;
  HANDLE event = NULL;
  void* apc_context;
  NTSTATUS status;

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
    if (event == NULL)
      return_error(-1);
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
        DWORD error = GetLastError();
        CloseHandle(event);
        return_error(-1, error);
      }

      status = iosb_ptr->Status;
    }

    CloseHandle(event);
  }

  if (status == STATUS_SUCCESS)
    return 0;
  else if (status == STATUS_PENDING)
    return_error(-1, ERROR_IO_PENDING);
  else
    return_error(-1, we_map_ntstatus_to_win_error(status));
}
