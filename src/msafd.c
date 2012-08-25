
#include <WinSock2.h>
#include <windows.h>

#include "msafd.h"
#include "ntapi.h"


static int afd_ntstatus_to_winsock_error(NTSTATUS status);
static sNtDeviceIoControlFile pNtDeviceIoControlFile;

int afd_init() {
  HMODULE ntdll; 
  
  ntdll = LoadLibraryW(L"ntdll.dll");
  if (ntdll == NULL)
    return -1;
  
  pNtDeviceIoControlFile = (sNtDeviceIoControlFile) GetProcAddress(ntdll, 
      "NtDeviceIoControlFile");
  if (pNtDeviceIoControlFile == NULL)
    return -1;

  return 0;
}


int WSAAPI afd_poll(SOCKET socket, AFD_POLL_INFO* info,
    OVERLAPPED* overlapped) { 
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
    event = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (event == NULL) {
      return SOCKET_ERROR;
    }
    apc_context = NULL;
  }

  iosb_ptr->Status = STATUS_PENDING;
  status = pNtDeviceIoControlFile((HANDLE) socket,
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
    /* If this is a blocking operation, wait for the event to become */
    /* signaled, and then grab the real status from the io status block. */
    if (status == STATUS_PENDING) {
      DWORD r = WaitForSingleObject(event, INFINITE);

      if (r == WAIT_FAILED) {
        DWORD saved_error = GetLastError();
        CloseHandle(event);
        WSASetLastError(saved_error);
        return SOCKET_ERROR;
      }

      status = iosb.Status;
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
      error = afd_ntstatus_to_winsock_error(status);
      break;
  }

  WSASetLastError(error);

  if (error == ERROR_SUCCESS) {
    return 0;
  } else {
    return SOCKET_ERROR;
  }
}


static int afd_ntstatus_to_winsock_error(NTSTATUS status) {
  switch (status) {
    case STATUS_SUCCESS:
      return ERROR_SUCCESS;

    case STATUS_PENDING:
      return ERROR_IO_PENDING;

    case STATUS_INVALID_HANDLE:
    case STATUS_OBJECT_TYPE_MISMATCH:
      return WSAENOTSOCK;

    case STATUS_INSUFFICIENT_RESOURCES:
    case STATUS_PAGEFILE_QUOTA:
    case STATUS_COMMITMENT_LIMIT:
    case STATUS_WORKING_SET_QUOTA:
    case STATUS_NO_MEMORY:
    case STATUS_CONFLICTING_ADDRESSES:
    case STATUS_QUOTA_EXCEEDED:
    case STATUS_TOO_MANY_PAGING_FILES:
    case STATUS_REMOTE_RESOURCES:
    case STATUS_TOO_MANY_ADDRESSES:
      return WSAENOBUFS;

    case STATUS_SHARING_VIOLATION:
    case STATUS_ADDRESS_ALREADY_EXISTS:
      return WSAEADDRINUSE;

    case STATUS_LINK_TIMEOUT:
    case STATUS_IO_TIMEOUT:
    case STATUS_TIMEOUT:
      return WSAETIMEDOUT;

    case STATUS_GRACEFUL_DISCONNECT:
      return WSAEDISCON;

    case STATUS_REMOTE_DISCONNECT:
    case STATUS_CONNECTION_RESET:
    case STATUS_LINK_FAILED:
    case STATUS_CONNECTION_DISCONNECTED:
    case STATUS_PORT_UNREACHABLE:
    case STATUS_HOPLIMIT_EXCEEDED:
      return WSAECONNRESET;

    case STATUS_LOCAL_DISCONNECT:
    case STATUS_TRANSACTION_ABORTED:
    case STATUS_CONNECTION_ABORTED:
      return WSAECONNABORTED;

    case STATUS_BAD_NETWORK_PATH:
    case STATUS_NETWORK_UNREACHABLE:
    case STATUS_PROTOCOL_UNREACHABLE:
      return WSAENETUNREACH;

    case STATUS_HOST_UNREACHABLE:
      return WSAEHOSTUNREACH;

    case STATUS_CANCELLED:
    case STATUS_REQUEST_ABORTED:
      return WSAEINTR;

    case STATUS_BUFFER_OVERFLOW:
    case STATUS_INVALID_BUFFER_SIZE:
      return WSAEMSGSIZE;

    case STATUS_BUFFER_TOO_SMALL:
    case STATUS_ACCESS_VIOLATION:
      return WSAEFAULT;

    case STATUS_DEVICE_NOT_READY:
    case STATUS_REQUEST_NOT_ACCEPTED:
      return WSAEWOULDBLOCK;

    case STATUS_INVALID_NETWORK_RESPONSE:
    case STATUS_NETWORK_BUSY:
    case STATUS_NO_SUCH_DEVICE:
    case STATUS_NO_SUCH_FILE:
    case STATUS_OBJECT_PATH_NOT_FOUND:
    case STATUS_OBJECT_NAME_NOT_FOUND:
    case STATUS_UNEXPECTED_NETWORK_ERROR:
      return WSAENETDOWN;

    case STATUS_INVALID_CONNECTION:
      return WSAENOTCONN;

    case STATUS_REMOTE_NOT_LISTENING:
    case STATUS_CONNECTION_REFUSED:
      return WSAECONNREFUSED;

    case STATUS_PIPE_DISCONNECTED:
      return WSAESHUTDOWN;

    case STATUS_INVALID_ADDRESS:
    case STATUS_INVALID_ADDRESS_COMPONENT:
      return WSAEADDRNOTAVAIL;

    case STATUS_NOT_SUPPORTED:
    case STATUS_NOT_IMPLEMENTED:
      return WSAEOPNOTSUPP;

    case STATUS_ACCESS_DENIED:
      return WSAEACCES;

    default:
      if ((status & (FACILITY_NTWIN32 << 16)) == (FACILITY_NTWIN32 << 16) &&
          (status & (ERROR_SEVERITY_ERROR | ERROR_SEVERITY_WARNING))) {
        /* It's a windows error that has been previously mapped to an */
        /* ntstatus code. */
        return (DWORD) (status & 0xffff);
      } else {
        /* The default fallback for unmappable ntstatus codes. */
        return WSAEINVAL;
      }
  }
}