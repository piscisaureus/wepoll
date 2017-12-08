#include "afd.h"
#include "error.h"
#include "nt.h"
#include "util.h"
#include "win.h"

#define FILE_DEVICE_NETWORK 0x00000012
#define METHOD_BUFFERED 0
#define AFD_POLL 9

#define _AFD_CONTROL_CODE(operation, method) \
  ((FILE_DEVICE_NETWORK) << 12 | (operation << 2) | method)

#define IOCTL_AFD_POLL _AFD_CONTROL_CODE(AFD_POLL, METHOD_BUFFERED)

#ifndef SIO_BASE_HANDLE
#define SIO_BASE_HANDLE 0x48000022
#endif

int afd_poll(SOCKET driver_socket,
             AFD_POLL_INFO* poll_info,
             OVERLAPPED* overlapped) {
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
  status = NtDeviceIoControlFile((HANDLE) driver_socket,
                                 event,
                                 NULL,
                                 apc_context,
                                 iosb_ptr,
                                 IOCTL_AFD_POLL,
                                 poll_info,
                                 sizeof *poll_info,
                                 poll_info,
                                 sizeof *poll_info);

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
    return_error(-1, RtlNtStatusToDosError(status));
}

static SOCKET _afd_get_base_socket(SOCKET socket) {
  SOCKET base_socket;
  DWORD bytes;

  if (WSAIoctl(socket,
               SIO_BASE_HANDLE,
               NULL,
               0,
               &base_socket,
               sizeof base_socket,
               &bytes,
               NULL,
               NULL) == SOCKET_ERROR)
    return_error(INVALID_SOCKET);

  return base_socket;
}

static ssize_t _afd_get_protocol_info(SOCKET socket,
                                      WSAPROTOCOL_INFOW* protocol_info) {
  int opt_len;
  ssize_t id;
  size_t i;

  opt_len = sizeof *protocol_info;
  if (getsockopt(socket,
                 SOL_SOCKET,
                 SO_PROTOCOL_INFOW,
                 (char*) protocol_info,
                 &opt_len) != 0)
    return_error(-1);

  id = -1;
  for (i = 0; i < array_count(AFD_PROVIDER_GUID_LIST); i++) {
    if (memcmp(&protocol_info->ProviderId,
               &AFD_PROVIDER_GUID_LIST[i],
               sizeof protocol_info->ProviderId) == 0) {
      id = i;
      break;
    }
  }

  /* Check if the protocol uses an msafd socket. */
  if (id < 0)
    return_error(-1, ERROR_DEVICE_FEATURE_NOT_SUPPORTED);

  return id;
}

WEPOLL_INTERNAL ssize_t afd_get_protocol(SOCKET socket,
                                         SOCKET* afd_socket_out,
                                         WSAPROTOCOL_INFOW* protocol_info) {
  ssize_t id;
  SOCKET afd_socket;

  /* Try to get protocol information, assuming that the given socket is an AFD
   * socket. This should almost always be the case, and if it is, that saves us
   * a call to WSAIoctl(). */
  afd_socket = socket;
  id = _afd_get_protocol_info(afd_socket, protocol_info);

  if (id < 0) {
    /* If getting protocol information failed, it might be due to the socket
     * not being an AFD socket. If so, attempt to fetch the underlying base
     * socket, then try again to obtain protocol information. */
    DWORD error = GetLastError();
    if (error != ERROR_DEVICE_FEATURE_NOT_SUPPORTED)
      return -1;

    afd_socket = _afd_get_base_socket(socket);
    if (afd_socket == INVALID_SOCKET || afd_socket == socket)
      return_error(-1, error);

    id = _afd_get_protocol_info(afd_socket, protocol_info);
    if (id < 0)
      return -1;
  }

  *afd_socket_out = afd_socket;
  return id;
}
