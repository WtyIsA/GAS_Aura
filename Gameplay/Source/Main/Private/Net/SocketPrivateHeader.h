
#ifndef __SOCKETPRIVATEHEADER__
#define __SOCKETPRIVATEHEADER__


#if PLATFORM_HAS_BSD_SOCKET_FEATURE_WINSOCKETS
    #include "Windows/WindowsHWrapper.h"
    #include "Windows/AllowWindowsPlatformTypes.h"

    #include <winsock2.h>
    #include <ws2tcpip.h>

    typedef int32 SOCKLEN;

#include "Windows/HideWindowsPlatformTypes.h"
#else
#if PLATFORM_SWITCH
#include "SwitchSocketApiWrapper.h"
#else
#include <unistd.h>
#include <sys/socket.h>
#if PLATFORM_HAS_BSD_SOCKET_FEATURE_IOCTL
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#endif
#include <netinet/in.h>
#include <arpa/inet.h>
#if PLATFORM_HAS_BSD_SOCKET_FEATURE_GETHOSTNAME
#include <netdb.h>
#endif
#if PLATFORM_HAS_BSD_SOCKET_FEATURE_NODELAY
#include <netinet/tcp.h>
#endif

#endif

#endif

#endif  //__SOCKETPRIVATEHEADER__