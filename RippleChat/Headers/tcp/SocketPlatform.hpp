// SocketPlatform.hpp
#pragma once

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
// Windows-specific definitions already included
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netdb.h>
#include <errno.h>
#include <string.h> // for memset

// Define Windows types for Linux
typedef int SOCKET;
struct WSADATA {
    int dummy;  // Dummy structure for Linux
};
#define MAKEWORD(x,y) 0
#define INVALID_SOCKET (-1)
#define SOCKET_ERROR (-1)
#define SOMAXCONN 128
#define IPPROTO_TCP 0
#define SD_SEND SHUT_WR
inline int closesocket(SOCKET s) {
    return close(s);
}
inline int WSAStartup(unsigned short, WSADATA*) {
    return 0;
}
inline void WSACleanup() {
}

// Socket address compatibility
typedef struct sockaddr SOCKADDR;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr_in* PSOCKADDR_IN;
typedef struct sockaddr* PSOCKADDR;

// Additional Windows compatibility
#define LPSOCKADDR struct sockaddr*
#define LPWSADATA WSADATA*
#define ioctlsocket ioctl
#define ULONG unsigned long
#define WORD unsigned short
#define DWORD unsigned long
#define LPVOID void*
#define FAR
#define INADDR_ANY (unsigned long)0
#endif

// Common socket function wrappers for both platforms
inline int platform_socket_error() {
#ifdef _WIN32
    return WSAGetLastError();
#else
    return errno;
#endif
}

inline void platform_socket_close(SOCKET s) {
#ifdef _WIN32
    closesocket(s);
#else
    close(s);
#endif
}